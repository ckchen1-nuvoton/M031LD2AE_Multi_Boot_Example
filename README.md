# M031LD2AE_Multi_Boot_Example

Implement a multi-boot system to boot from different applications in APROM
The system allocates the 64KB APROM into two equal 32KB partitions:  
Boot0 (Code0): Located at base 0x0000_0000 (Size: 32KB)   
Boot1 (Code1): Base offset at 0x8000 (Size: 32KB)   
Bootcode (LDROM): Located at 0x0010_0000 (Size: 2KB)   

	<img width="60%" alt="image" src="https://github.com/user-attachments/assets/145d7222-ce40-45b3-9ba9-3f056929eb6a" />

1. Compilation Steps (in Keil)
	Select and compile FMC_Boot0 to generate FMC_Boot0.bin.  
	Select and compile FMC_Boot1 to generate FMC_Boot1.bin.  
	Select and compile FMC_BootLD to generate FMC_BootLD.bin.

	<img width="30%" alt="image" src="https://github.com/user-attachments/assets/57dcfe72-600b-4b80-91e9-b91015da3422" />

3. Programming via NuMicro ICP Tool 
	Open the ICP Tool and enable Multi-File Mode.  
	Click on APROM and import the binary files:   
	Import FMC_Boot0.bin with offset 0x0.  
	Import FMC_Boot1.bin with offset 0x8000.  
	Click on LDROM and import FMC_BootLD.bin.

	<img width="60%" alt="image" src="https://github.com/user-attachments/assets/6ff175a4-7757-4f8e-98a8-5ce45635ea26" />
	<img width="40%" alt="image" src="https://github.com/user-attachments/assets/fcde6e03-ee33-449f-997f-8689e2210695" />


	Go to Config Bits / Setting and set the Chip Booting Selection to LDROM with IAP. 
	In the Programming section, check APROM, LDROM, and Config, then click Start to burn the firmware.  

	<img width="40%" alt="image" src="https://github.com/user-attachments/assets/f2c6af45-c8ba-4276-9f0a-3e4060a5328f" />
	<img width="40%" alt="image" src="https://github.com/user-attachments/assets/1a85c665-0b2c-41cf-92f9-dbf76793f45e" />


5. Execution Results

	<img width="30%" alt="image" src="https://github.com/user-attachments/assets/749eebfd-d99a-4b1e-bf10-0825df3da901" />
   
