# Sericat
An overly complicated alternative to I2C: inspired by Ethercat, using a serial "ring"

## "Requirements" / use case
Basically, I want a to compose a set of microcontrollers together in such a way that a single "master" board can poll input state from all of them, (and potentially command outputs, though that's not part of my initial use case). I want to be able to add peripheral devices to the network without having to re-flash anything. This means I can't have slave devices owning "hard-coded" addresses, but rather need them to "discover" their place in the network at setup/run-time. 
In addition, these slave devices need to be able to have different PDOS.

Admitedly, this is entirely overcomplicated and needlessly contrived. I2C or a dumber serial setup would be fine, and re-flashing devices is probably way less work than what i'm going to to make this work. Whatever, this is for fun.

Also, if i get this right, i can more reasonably use this codebase to build stuff for other people who wouldn't be up for re-flashing devices with new addresses etc.


## Architecture plan
This is heavily inspired by EtherCAT, so i'm gonna steal a lot of terminology.

What i want to do is this:

### Process Frame
This protocol will provide cyclic access to a "frame" of data. This frame will be sent by the master device along the serial ring, and will be pased from slave to slave and then back to the master: as the frame is received by each device, it is retrasmitted to the next device in the ring. Elements in the from are "Process Data Objects" (`PDO`), with each PDO being either a target state ("Output" or `RxPDO`) received by a particular slave, or a state query ("Input" or `TxPDO`) transmitted back tothe master. Which `TxPDO`s and `RxPDO`s included in the frame is determined by each slave's configuration: different slaves can have differnt `PDO`s.


### SDOs
Each slave device will have a data map of "SDO" (software data object) entries. Each SDO has data in it, with datatype being one of the following currently supported:

- BOOL
- INT_32
- FLOAT
- STRING

At the lowest level, the master will have the ability to read/write data to a slave's PDOs by sending an address and a value. This will happen over serial, so all data needs to be able to be turned into chars. I'm gonna accomodate this by just storing all data as std::strings, which'll handle dynamic data sizing for me.

SDOs will be statically instantiated on the slave device, in some kind of SDO-MAP object.

### SDO Map
Each slave device will have 3 sections of SDOs:
- Config
  - PDO assignment
    - RxPDO assingment
    - TxPDO assignemnt
  - device-specific config 
- PDOS
  - RxPDOS (Inbox)
  - TxPDOs (Outbox)

### Config
The sdos in the config section will provide the master a way to modify the behavior of the slave at runtime, or at least an setup-time. For example, the config section might have registers that let you set debounce time for each input, or modify operation mode or something.
In addition, the "PDO assignment" SDOs will be how the master device tells the slave which inputs and outputs to read from/ write to the cyclic "process" frame. `RxPDO assignemnt` will determine which `PDO`s are *received* _from the master_, while `TxPDO assignment` will determine which `PDO`s are *transmitted* _to the master_ . 

