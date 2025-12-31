.. _tinyusb_msc_u_disk_explorer:

TINYUSB HOST MSC U DISK EXPLORER
==================================

Overview
--------

- This sample project demonstrates U disk access operations (Recommendation: U disk formatting parameters FAT32/32KB).

  .. image:: ./doc/UDisk_Format.png
     :alt: UDisk_Format

- MCU acts as a file manager, where you can access the U disk through commands in the Console. Supported commands are as follows:

.. code-block:: text

 * help
        Print list of commands
 * cat
        Usage: cat [FILE]...
        Concatenate FILE(s) to standard output..
 * cd
        Usage: cd [DIR]...
        Change the current directory to DIR.
 * cp
        Usage: cp SOURCE DEST
        Copy SOURCE to DEST.
 * ls
        Usage: ls [DIR]...
        List information about the FILEs (the current directory by default).
 * pwd
        Usage: pwd
        Print the name of the current working directory.
 * mkdir
        Usage: mkdir DIR...
        Create the DIRECTORY(ies), if they do not already exist..
 * mv
        Usage: mv SOURCE DEST...
        Rename SOURCE to DEST.
 * rm
        Usage: rm [FILE]...
        Remove (unlink) the FILE(s).

Hardware Setup
--------------

- Use a USB Type-C cable to connect the PC USB port and the PWR DEBUG port

- Use a USB Type-C to USB-A cable to connect the U disk and the development board USB0 port or USB1 port

Running the example
---------------------

- When the project runs correctly, it will display the U disk information:

.. code-block:: text

  TinyUSB Host MassStorage Explorer Example
  A MassStorage device is mounted
  IS917    innostor rev 1.00
  Disk Size: 7500 MB

- After pressing Enter in the console, the `>` prompt will appear, and then you can operate the U disk. Input the help command to display the supported command list.

.. code-block:: text

  >
  > help
   * help
          Print list of commands
   * cat
          Usage: cat [FILE]...
          Concatenate FILE(s) to standard output..
   * cd
          Usage: cd [DIR]...
          Change the current directory to DIR.
   * cp
          Usage: cp SOURCE DEST
          Copy SOURCE to DEST.
   * ls
          Usage: ls [DIR]...
          List information about the FILEs (the current directory by default).
   * pwd
          Usage: pwd
          Print the name of the current working directory.
   * mkdir
          Usage: mkdir DIR...
          Create the DIRECTORY(ies), if they do not already exist..
   * mv
          Usage: mv SOURCE DEST...
          Rename SOURCE to DEST.
   * rm
          Usage: rm [FILE]...
          Remove (unlink) the FILE(s).
  >

- By entering other commands, you can also operate the U disk.