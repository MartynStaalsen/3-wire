from collections import deque
import time
import threading
import atexit


class duplex_pynode:
    index = 0

    def __init__(self, upstream):
        self.upstream = upstream
        self.q0 = deque()
        self.q1 = deque()

        self.enabled = False

        self.index = pynode.index
        self.data = f'{self.index}_{time.time()}'

        pynode.index += 1
        self.rx_0_mutex = threading.Lock()
        self.rx_1_mutex = threading.Lock()

        # spawn thread to spin logic_loop
        self.thread = threading.Thread(target=self.logic_loop)
        atexit.register(self.closeout)
        self.thread.start()

    # closes thread on exit
    def closeout(self):
        self.enabled = False
        self.thread.join()

    def poll_inputs(self):
        time.sleep(0.01)  # simulate delay

        self.data = f'{self.index}_{time.time()}'

    # receive from master
    def receive_0(self, data):
        self.rx_0_mutex.acquire()
        # print(f'{self.index} received {data} on 0')
        self.q0.append(data)
        self.rx_0_mutex.release()

    # receive from downstream
    def receive_1(self, data):
        self.rx_1_mutex.acquire()
        # print(f'{self.index} received {data} on 1')
        self.q1.append(data)
        self.rx_1_mutex.release()

    # send a byte upstream
    def transmit_0(self, data):
        # print(f'{self.index} transmitting {data} on 0')
        self.upstream.receive_1(data)

    def logic_loop(self):
        # print(f'starting logic loop for node {self.index}')
        self.enabled = True
        while self.enabled:
            # send any passthru data, then mine
            self.poll_inputs()  # "read" state (randomize value)

            if not len(self.q0) == 0:
                # "read" all input from master (discard for now)
                rec_data = self.q0.popleft()
                self.q0.clear()
                self.transmit_0(self.data + '_' + rec_data)  # transmit state to upstream


            # retransmit any data from downstream
            while len(self.q1) > 0:
                self.transmit_0(self.q1.popleft())

            time.sleep(0.005)


class duplex_pynode_master:
    def __init__(self):
        self.nodes = []
        self.q = deque()

    def add_node(self, node):
        self.nodes.append(node)

    def send(self, chr):
        for n in self.nodes:
            n.receive_0(chr)

    def receive_1(self, chr):
        self.q.append(chr)
