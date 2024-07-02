#!/bin/bash

./CHQStage.py --stage=1 --base=0xc000 Stage1.bin > Stage1.ctl && sna2skool.py --hex --ctl Stage1.ctl --page 1 --start 49152 --end 65536 ./build/ChaseHQ-128K.pristine.z80 > Stage1.skool
./CHQStage.py --stage=2 --base=0xe000 Stage2.bin > Stage2.ctl && sna2skool.py --hex --ctl Stage2.ctl --page 1 --start 49152 --end 65536 ./build/ChaseHQ-128K.pristine.z80 > Stage2.skool
./CHQStage.py --stage=3 --base=0xc000 Stage3.bin > Stage3.ctl && sna2skool.py --hex --ctl Stage3.ctl --page 6 --start 49152 --end 65536 ./build/ChaseHQ-128K.pristine.z80 > Stage3.skool
./CHQStage.py --stage=4 --base=0xe000 Stage4.bin > Stage4.ctl && sna2skool.py --hex --ctl Stage4.ctl --page 6 --start 49152 --end 65536 ./build/ChaseHQ-128K.pristine.z80 > Stage4.skool
./CHQStage.py --stage=5 --base=0xc000 Stage5.bin > Stage5.ctl && sna2skool.py --hex --ctl Stage5.ctl --page 7 --start 49152 --end 65536 ./build/ChaseHQ-128K.pristine.z80 > Stage5.skool
