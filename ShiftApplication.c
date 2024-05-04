#include <Uefi.h>
#include <stdlib.h>
#include <stdio.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/FileHandleLib.h>
#include <Library/PrintLib.h>
#include <Library/BaseLib.h>
#include <Protocol/ShellParameters.h>
#include <Protocol/SimpleFileSystem.h>


EFI_STATUS EFIAPI UefiMain (IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable){ //Входная точка 

 EFI_STATUS Status;
 EFI_SHELL_PARAMETERS_PROTOCOL *ShellParameters;
 EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *SimpleFsys;
 EFI_FILE_PROTOCOL *File;
 EFI_FILE_PROTOCOL *FileDir;
 EFI_HANDLE *Handles;
 UINTN Count; //количество параметров в строке (хотя, их и так всегда 3 при корректном вводе);
 CHAR16 **Parameter; //Массив параметров из строки
 CHAR16 *FileName; //Имя файла
 CHAR16 *ShiftParameter; //Параметр сдвига
 UINTN ShiftParameterNumeric; //Параметр сдвига в числовом формате 
 UINTN HandleCount; //Подсчет хэндлов
 UINT8 Buffer[1024]; //Буффер для информации с файла
 UINTN Tmp; //Для хранения размера буффера
 CHAR16* EndPOinter = NULL; //указатель на конец строки
 BOOLEAN Checker = FALSE; //Для проверки наличия минуса перед параметром сдвига

//нашли экземпляр протокола для получения параметров строки
 Status = gBS->HandleProtocol(ImageHandle, &gEfiShellParametersProtocolGuid, (VOID**)&ShellParameters); 
 if(EFI_ERROR(Status)){
    Print(L"Error on Shell Parameters Protocol\n");
    return Status;
 }
 Print(L"SHELL PARAMETERS DONE ====================\n\n");

//получаем параметры и количество аргументов (хоть и всегда 3 при корректном вводе)
 Parameter = ShellParameters->Argv;
 Count = ShellParameters->Argc;

//инициализация имени файла и параметра сдвига
 FileName = Parameter[1];
 ShiftParameter = Parameter[2];
 Print(L"%d params\n filename is %s\n shell parameter is %s\n\n",Count, FileName, ShiftParameter);
 
//Конвертация CHAR16 в UINTN и определение чекера минуса перед параметром 
 if(StrLen(ShiftParameter)>1){
   if(ShiftParameter[0] == '-'){
        Checker = TRUE;
        Status = StrDecimalToUintnS(&ShiftParameter[1], &EndPOinter, &ShiftParameterNumeric);
        if(EFI_ERROR(Status)){
          Print(L"BAD CONVERT\n");
          return Status;
        }
   } else {
      Print(L"Incorrect argument\n");
      return EFI_INVALID_PARAMETER;
   }
 } else {
    Status = StrDecimalToUintnS(&ShiftParameter[0], &EndPOinter, &ShiftParameterNumeric);
    Checker = FALSE;
    if(EFI_ERROR(Status)){
      Print(L"BAD CONVERT\n");
      return Status;
    }
 }
 Print(L"SHIFT IS %d\n", ShiftParameterNumeric);
 Print(L"ARGUMENTS DONE ====================\n\n");


//определили пул протоколов для симпл файл систем
 Status = gBS->LocateHandleBuffer(ByProtocol, &gEfiSimpleFileSystemProtocolGuid, NULL, &HandleCount, &Handles);
 if(EFI_ERROR(Status)){
   Print(L"Error on Simple File System Protocol\n");
   return Status;
 }
 Print(L"STARTING LOCATE HANDLE DONE ====================\n\n");

//ищем нужный файл с нашим названием посредством перебора
 for(UINTN i = 0; i < HandleCount; i++){
   Status = gBS->HandleProtocol(Handles[i], &gEfiSimpleFileSystemProtocolGuid, (VOID**)&SimpleFsys);
   if(!EFI_ERROR(Status)){
      Status = SimpleFsys->OpenVolume(SimpleFsys, &FileDir);
      if(!EFI_ERROR(Status)){
         Status = FileDir->Open(FileDir, &File, FileName, EFI_FILE_MODE_READ, 0);
         if (!EFI_ERROR(Status)){
            break;
         }
      }
   }
 }
 if(EFI_ERROR(Status)){
   Print(L"Error on Locating/Handling protocol\n");
   return Status;
 }
 Print(L"LOCATING HANDLE DONE ====================\n\n");

//Открыть директорию
 Status = SimpleFsys->OpenVolume(SimpleFsys, &FileDir);
 if(EFI_ERROR(Status)){
   Print(L"Error on opening volume\n");
   return Status;
 }
 Print(L"OPENING VOLUME DONE ====================\n\n");
 //Открыть файл для чтения и записи
 Status = FileDir->Open(FileDir, &File, FileName, EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE, 0);
 if(EFI_ERROR(Status)){
   Print(L"Error on opening File\n");
   return Status;
 }
 Print(L"OPENING FILE DONE ====================\n\n");
 Status = File->Read(File, &Tmp, &Buffer); //читаем файл
  if(EFI_ERROR(Status)){
   Print(L"Error on reading File\n");
   return Status;
 }
 Print(L"READING FILE DONE ====================\n\n");

 for(UINTN i = 0; i < Tmp; i++){
   Print(L"%02x\t is %c\n", Buffer[i], Buffer[i]); //выводим оригинальное содержимое файла
 }
  Print(L"PRINTING ORIGINAL DONE ====================\n\n");

//циклический побитовый сдвиг влево и вправо в зависимости от значения чекера (шифтпараметр беззнаковый, а как-то переопределять го
//уже глупо, так что пойдем "булевым" методом)
  UINT8 PREV_CHUNK_BIT = 0;
  UINT8 CURR_CHUNK_BIT = 0;
  if(!Checker){
    for(UINTN j = 0; j < ShiftParameterNumeric; j++){
      PREV_CHUNK_BIT = 0;
      CURR_CHUNK_BIT = 0;
      for (UINTN i = 0; i < Tmp; ++i) {
          CURR_CHUNK_BIT = Buffer[i] & 0x01; 
          Buffer[i] = (Buffer[i] >> 1) | (PREV_CHUNK_BIT << 7); 
          PREV_CHUNK_BIT = CURR_CHUNK_BIT; 
      }
    } 
  } else {
    for(UINTN j = 0; j < ShiftParameterNumeric; j++){
      PREV_CHUNK_BIT = 0;
      CURR_CHUNK_BIT = 0;
      for (UINTN i = 0; i < Tmp; ++i) {
          CURR_CHUNK_BIT = Buffer[i] & 0x80; 
          Buffer[i] = (Buffer[i] << 1) | (PREV_CHUNK_BIT >> 7); 
          PREV_CHUNK_BIT = CURR_CHUNK_BIT; 
      }
    } 
  }


//Отправили указатель на начало файла
 Status = File->SetPosition(File, 0);
 if (EFI_ERROR(Status)){
   Print(L"ERROR ON SETTING POSITION\n");
 }
 Print(L"POSITION SETTING 0 DONE ====================\n\n");
 
 //Запись в файл
 Status = File->Write(File, &Tmp, &Buffer);
 if (EFI_ERROR(Status)){
   Print(L"ERROR ON WRITING\n");
 }
 Print(L"WRITING INTO FILE DONE ====================\n\n");

 for(UINTN i = 0; i < Tmp; i++){
  Print(L"%02x\t is %c\n", Buffer[i], Buffer[i]);
 }
 Print(L"\n");
 Print(L"PRINTING NEW LINE DONE ====================\n\n");

//Осовобождение объекта, содержащего спсиок хэндлов
 Status = gBS->FreePool(Handles);
 if (EFI_ERROR(Status)){
   Print(L"ERROR ON FREEING HANDLES\n");
 }
 //Закрытие файла
 File->Close(File);
 
 Print(L"CLOSING FILE DONE ====================\n\n"); 
 return EFI_SUCCESS;
}
