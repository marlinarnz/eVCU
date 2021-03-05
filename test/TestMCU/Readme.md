# MCU component testing

This step concerns testing of the APEV528 Controller MCU under use of a Raspberry Pie with CAN board. The bus is continously scanned in human readable language. The `can_decoder.py` file defines a `Decoder` class which parses CAN message structures from the given Excel file. The `mcu_test_suite.py` creates a graphical user interface with `tkinter` and monitors the bus.