
# UEFI-based shifting application 

> ### UEFI-based application for shifting bits in a file.

> [!IMPORTANT]
> This application was created using @tianocore/edk2 tools and UEFI specification, so, if you have an intention to modify and rebuild this little training project for your purposes, you have to set up edk2 according to guidelines, posted on tianocore team's [GitHub page](https://github.com/tianocore/tianocore.github.io/wiki/Getting-Started-with-EDK-II).

### To make this application work in your UEFI Shell you should:
- ### Make sure that your system supports UEFI;
- ### Make sure that your processing file has `.txt` format (app is working only with text files for now);
- ### Make sure that you have put `.efi` and `.txt` file in your fs0: volume (boot directory like admin:///boot/efi) in a same place[^1];
- ### Enter the UEFI Shell, move to fs0: volume (and then to the directory having your `.efi` file, if you did't put it in the boot one) and write the command like that down: `ShiftApplication.efi test.txt 3`, where:
  - `ShiftApplication.efi` - application's name[^2];
  - `test.txt` - file's name[^3];
  -  `3` - shifting parameter[^4].

### To make this application work on VM[^5] you should build and implement the `OVMF.fd` file in it (just put `-bios path_to_your_OVMF/OVMF.fd` flag in VM's settings).
___
## Application's working stages:
1. ### Reading the input line, handling the file's name and shifting parameter;
2. ### Finding and opening the file (inr reading and writing mode);
3. ### Writing the original into the UINT8 Buffer;
4. ### Shifting bits in the Buffer;
5. ### Rewriting the file with a new information;
6. ### Freeing some of the memory pool and closing the file.
_____
## ERRORS & BUGS CATCHING JOURNAL
> ### The list of known issues. Ticked box means the solved problem.
- [ ] Unable to operate non-txt files
- [ ] Unable to opearate relativly big files (like over 200kB)
- [ ] Unable to process incorrect parameter input
_____

[^1]:Not neccesary, but it would be easier to find the file.
[^2]:You can change it in the `.inf` file and enter the correct path in the `MdeModulePkg.dsc` to rebuild the project successfuly.
[^3]:You can name a file whichever you want.
[^4]:You can set any parameter you want, including a negative parameter, but in the range of [-8;8].
[^5]:It's strongly recommended to use QEMU for virtualizing.
