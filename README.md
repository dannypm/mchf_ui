# mcHFx

-- mcHF rev 0.8 UI board firmware --

Note: This code is still somehow experimental. For older mcHF board revisions, please use the better supported 
fork of the code here: https://github.com/df8oe/UHSDR

-------------------------------------------------------------------------------------------------------------
How to compile:

- Download SW4STM32 studio (http://www.openstm32.org) and install

- On first run set your Workspace directory to something in MyDocuments,
not the mcHF project directory! In Eclipse, unlike other IDEs, the Workspace
is settings holder, not projects grouping method!

- Import the mcHF project from local dir or GitHub directly(point the path to
firmware\mchf_ui\project\mchf-pro and make sure mchf-pro is checked)

- Open mchf_pro_board.h, then from the Project Explorer on the left, Right click on the
project name(mchf-pro) then go to Build Configurations->Set Active->STM32H7

- If #define CHIP_H7 is not highlighted after few seconds, do change this:
Window->Preferences->Indexer->Build configuration for the indexer->Use active build configuration

- Press Control-B to build, Run should be able to flash the Elf file to the board, if saved configurations
are detected(mchf-pro_f7.cfg or mchf-pro_h7.cfg), if not you have to make fresh one from the 
Run->Run Configuraions...->Ac6 STM32 Debugging, then Right click New

- It is possible to customize Eclipse to your liking - dictionary file, compile with F7, run with F9, etc
but needs lots of Google search and digging into menues

-------------------------------------------------------------------------------------------------------------
Produced binaries and compilation speed:

If using external builder, full rebuild is extremly slow, but i use to produce .elf and .bin file at the same
time. If faster compilation is needed, you can switch to internal builder and select unlimited parallel threads
(Project Explorer window on left, right click on project name, Properties, then C/C++ Build, Builder Settings ->
Builder Type (external or internal), second tab - Behaviour 'Enable parallel build' check box)

______________________________________________________________
All mcHF downloads here: http://www.m0nka.co.uk/?page_id=5269

--------------------------------------------------------------
Krassi Atanassov, M0NKA
mcHF project, 2012 - 2019
