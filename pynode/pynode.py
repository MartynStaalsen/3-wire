from collections import deque
import time
import threading
import atexit


class pynode_master:
    def __init__(self):
        self.nodes = []
        self.input_buffer = deque()
        self.data = []
        self.enabled = True
        self.thread = threading.Thread(target=self.loop)
        atexit.register(self.closeout)

    def receive(self, chr):
        self.input_buffer.append(chr)

    def setup(self):
        data_len = []
        while True:
            ch = self.input_buffer.popleft()
            if ch == '!':
                break
            elif ch == '_':
                ch = self.input_buffer.popleft()
                data_len = [int(ch)] + data_len

        print(f'found {data_len} nodes')
        for i in data_len[::-1]:
            self.data = [0 for i in range(i)]

        self.start_loop()

    def blocking_pop(self):
        while len(self.input_buffer) == 0:
            time.sleep(0.005)
        return self.input_buffer.popleft()

    def loop(self):
        while self.enabled:
            ch = self.blocking_pop()
            if ch == '-':
                ch = self.blocking_pop()
                node_index = int(ch)
                for i in range(len(self.data)):
                    self.data[i] = int(self.blocking_pop())
                print(f'node {node_index} data: {self.data}')
            else:
                assert False, 'unexpected data'

    def start_loop(self):
        self.thread.start()

    # closes thread on exit
    def closeout(self):
        self.enabled = False
        self.thread.join()


class pynode:
    index = 0

    def __init__(self, downstream, bytes=1):
        self.downstream = downstream
        self.q = deque()

        self.enabled = False

        self.static_index = pynode.index
        self.index = None

        pynode.index += 1
        self.tx_mutex = threading.Lock()
        self.rx_mutex = threading.Lock()

        self.inputs = [0 for _ in range(bytes)]
        self.last_inputs = self.inputs.copy()

        # spawn thread to spin logic_loop
        atexit.register(self.closeout)
        self.thread = threading.Thread(target=self.do)
        self.thread.start()

    def do(self):
        self.setup()
        self.logic_loop()

    def blocking_pop(self, timeout=None):
        first = time.time()
        while len(self.q) == 0:
            time.sleep(0.005)
            if timeout is not None and time.time() - first > timeout:
                return None
        return self.q.popleft()

    def peek(self):
        if len(self.q) == 0:
            return None
        return self.q[-1]

    def setup(self):
        # start by registering index
        # do this by sending a single byte
        # forwarding all traffic
        # then counting total bytes forwarded after some reasonable timeout

        self.transmit(ord('!'))
        counts = 0
        while True:
            ch = self.blocking_pop(timeout=1)

            if ch is None:
                break

            counts += 1
            self.transmit(ch)
        self.index = counts

        print(f'{self.static_index} my index is {self.index}')

        # now need to report data size to master
        # do this by sending my id plus the number of inputs
        self.transmit([ord('['), self.index, len(self.inputs), ord(']')])
        self.retransmit(opench='[', closech=']', packets=self.index)  # retransmit all setup packets

    def print(self):
        print(f'\t{self.static_index}/{self.index} inputs: {self.inputs}')

    # closes thread on exit
    def closeout(self):
        self.enabled = False
        self.thread.join()

    def poll_inputs(self):

        if self.inputs != self.last_inputs:
            self.last_inputs = self.inputs.copy()
            self.transmit([ord('{'), self.index] + self.inputs + [ord('}')])

    # receive from downstream
    def receive(self, byte: int):
        assert byte < 256
        assert byte >= 0

        print(f'{self.static_index} received {chr(byte)}')
        self.rx_mutex.acquire()
        self.q.append(byte)
        self.rx_mutex.release()

    # send a byte upstream
    def transmit(self, bytes: int):
        # if bytes is a single byte, convert to list
        if type(bytes) == int:
            bytes = [bytes]

        print(f'{self.static_index} transmitting {[chr(b) for b in bytes]}')
        self.tx_mutex.acquire()  # mutex ensures all bytes transmit together
        for byte in bytes:
            self.downstream.receive(byte)
            assert byte < 256
            assert byte >= 0

        self.tx_mutex.release()

    def retransmit(self, opench='{', closech='}', packet_timeout=1, read_timeout=0.1, packets=None):
        packet_cnt = 0
        while True:
            d = self.blocking_pop(timeout=read_timeout)
            if d is None:
                break
            if d == ord(opench):
                self.transmit(d)
                # keep polling until } to ensure messages aren't spliced with my data
                while d != ord(closech):
                    d = self.blocking_pop(timeout=packet_timeout)
                    assert d is not None, f'node {self.static_index} timed out waiting for {closech}'
                    self.transmit(d)
                packet_cnt += 1
                if packets is not None and packet_cnt >= packets:
                    break
            else:
                print(f'{self.static_index} retransmitting unexpected byte {d}')
                self.transmit(d)

    def logic_loop(self):
        # print(f'starting logic loop for node {self.index}')
        self.enabled = True
        while self.enabled:
            # send any passthru data, then mine
            self.poll_inputs()  # "read" state (randomize value)

            # retransmit any data from downstream
            self.retransmit()

            time.sleep(0.005)
