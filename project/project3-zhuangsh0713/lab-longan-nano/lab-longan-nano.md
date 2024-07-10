Author: Songhui Cao `<caosh2022>`

This material derives directly from Lab 12, which has been verified by: Luojia Hu `<hulj>`.

# CS110@ShanghaiTech 2024 Spring Lab 11 (Longan Nano Part)

To those Windows protagonist: we CS110 TAs have noticed your determination sticking to Windows for CA1. Lab 11 Longan Nano Part, Lab 12 and Project 3 will come with official Windows support.

## Step 0: Identify Objectives for Lab 11 (Longan Nano Part)

1. Install necessary software so that you can download our sample firmware to Longan Nano. (Step 1)
   - Estimated Time: 3 minutes for installing DFU tool; \~20 minutes for installing platformio (optional)
2. Check if the Longan Nano board and its peripheral is working. (Step 2)
   - Estimated Time: 20 minutes, including TA's recitation.

## Step 1: Install Necessary Software

Download and install the Longan development kit for CA1 from here: https://epan.shanghaitech.edu.cn/l/KFjNMO (Link valid until June 30, 2024). The downloaded compressed archive contains installation guide.

**For Windows Users**

It is possible to complete Lab 11 (Longan Nano Part) without installing platformio. If you don't have enough time to install platformio in the lab, installing only GD32 MCU Dfu Tool (together with its driver) in the compressed archive suffices for this lab. However, you do need platformio and the whole Longan development kit for CA1 to complete Lab 12 and Project 3.

**For Linux Users**

It is possible to complete Lab 11 (Longan Nano Part) without installing platformio. If you don't have enough time to install platformio in the lab, inflating `dfu-util` from the compressed archive suffices for this lab. However, you do need platformio and the whole Longan development kit for CA1 to complete Lab 12 and Project 3.

## Step 2: Download Sample Firmware to Device

### Longan Nano Basics

- In consideration that incorrect connection between Longan Nano and our peripheral PCB board might cause irreversible damage to the Longan Nano board, there will not be discussions about Longan Nano basics here. Instead, in order to ensure your understanding about the board, the discussion about Longan Nano will be held in the lab. Lab TAs will talk about Longan Nano basics during Lab 11.
- Reference material: the following image and https://longan.sipeed.com/zh/ .
  ![nano.png](https://toast-lab.sist.shanghaitech.edu.cn/courses/CS110@ShanghaiTech/Spring-2023/labs/lab12/Lab%2012%20-%20Computer%20Architecture%20I%20-%20ShanghaiTech%20University_files/nano.png)

### Workflow and Explanation

**Enter DFU mode (on Longan Nano):**

1. Hold BOOT0 button (and do not release)
2. Hold RESET button (and do not release)
3. Release RESET button
4. Release BOOT0 button

- Notes and Explanation:
  - The temporal ordering of pressing and releasing buttons is of vital importance on entering DFU mode. Therefore, if the attempt of entering DFU fails, please first try repeating the above procedure before posting on Piazza.

**Download (write) to device (on PC):**

- On Windows: use `GD32 MCU Dfu Tool_v3.8.1.5784` we provided.

  - Select `Download to Device`, and open the `.bin` file (in this case `lab-longan-nano-firmware-distributable.bin`)
  - Keep `@0x80000000` as default. Generally you do not need to *Verify After Download* or *Erase All*.
  - Make sure the `GD32 MCU Dfu Tool_v3.8.1.5784` tool can see your device.
  - Press `OK` and let `GD32 MCU Dfu Tool_v3.8.1.5784` download your firmware to device.

- On Linux: use `dfu-util` we provided.

  - Copy `dfu-util` from `linux-version` to some location, for now we name this location `<lab root>`. Then `chmod +x <lab root>/dfu-util`.

  - ```
    cd <lab root>
    ./dfu-util -a 0 --dfuse-address 0x08000000:leave -D path_to_lab-longan-nano-firmware-distributable.bin
    ```

  - If the above command complains that the device cannot be opened, but your device is in DFU mode, use `sudo` to run `dfu-util`.
  
  - The above command will complain `Invalid DFU suffix signature` and `dfu-util: Error during download get_status`. Ignore those two errors.

**Leave DFU mode (Windows version: on PC/Longan Nano; Linux version: on Longan Nano)**

- For Windows users: Select *Leave DFU* on PC in `GD32 MCU Dfu Tool_v3.8.1.5784` suffices. You can also press RESET key on Longan Nano.
- For Linux users: On Longan Nano, press RESET key.

### Workflow Completion Self-Check

Reference Video: See `lab-longan-nano.MOV`.

The following steps will guide you perform a self-check on the above steps.

Press RESET, then push joystick up, down, left, right, and towards the middle. You should see blue character `U`, `D`, `L`, `R`, and `C` respectively.

After that, press the the two buttons on the left side. You should see `SW1` and `SW2` respectively.

If all of the above steps succeed, then you have successfully completed Steps 0, 1, and 2.
