import pynode
import time


class dumb_master:
    def __init__(self):
        self.nodes = []
        self.q = []

    def receive(self, byte):
        print(f'master received {chr(byte)}')
        # if byte is a non-ascii character, convert to binary and append as string
        if byte < 32 or byte > 126:
            self.q.append(bin(byte))
            self.q.append('-')

        else:

            self.q.append(chr(byte))


master = dumb_master()

N = 2
network = [master]
for i in range(N):
    new_node = pynode.pynode(downstream=network[-1])
    network.append(new_node)

network[1].inputs[0] = 8
time.sleep(0.2)
network[2].inputs[0] = 2

time.sleep(4)
received = []
while len(master.q) > 0:
    received.append(master.q.pop())
received.reverse()
rec_str = ''.join([r for r in received])
print(rec_str)



# master.setup()
# time.sleep(5)
# print(f'{master.data}')
# for n in network[1:]:
#     n.print()
# print()
# time.sleep(0.2)

# network[1].inputs[1] = 1
# time.sleep(0.01)
# print(f'{master.data}')
# for n in network[1:]:
#     n.print()
# print()


# network[N].inputs[N] = 1
# time.sleep(0.01)
# print(f'{master.data}')
# for n in network[1:]:
#     n.print()
# print()


for n in network[1:]:
    n.closeout()

# master.closeout()
