.. _lv_demo_music:

lv_demo_music
==========================

Overview
--------

The music player demo shows what kind of modern, smartphone-like user interfaces can be created on LVGL， and can play music.

Please refer to `official lv_demos github <https://github.com/lvgl/lv_demos>`_

Project Configuration
-----------------------

- The audio output device is auto-detected when this sample's ``app.yaml`` declares a ``board_audio_codec`` dependency and the board declares both ``board_audio_codec`` and a ``board_codec_*`` feature. Otherwise DAO is used.

- To override the auto-detection, set ``CONFIG_CODEC`` before ``find_package`` in ``CMakeLists.txt``:

  - ``set(CONFIG_CODEC 1)`` — force Codec output
  - ``set(CONFIG_CODEC 0)`` — force DAO output

- The Audio Codec type is automatically matched with the hardware based on the board feature (e.g., ``board_codec_es8389``). To specify an audio codec type manually, set the following in ``CMakeLists.txt``: ``set(CONFIG_CODEC_NAME "wm8960")``.

Board Settings
--------------

Attach LCD panel to the board LCD interface.

Attach MicroSD card that stores songs in WAV format into the card slot;

According to project configuration, connect speaker to  :ref:`DAO <board_resource>`  interface if using DAO as player, connect headphone to  :ref:`headphone <board_resource>`  interface if using audio codec as player.

Known issues
------------

When using DAO to play some audio, it may generate noise.

Running the Demo
----------------

lvgl music will be shown on the LCD panel and can operate play/pause, next, previous.

Remark
------

- As framebuffer and mp3 decoder occupies a large amount of memory, please specify HPM_BUILD_TYPE as flash_sdram_xip. The following commands can be executed:


.. code-block:: text

   cmake -GNinja -DBOARD=hpm6750evkmini -DHPM_BUILD_TYPE=flash_sdram_xip -DCMAKE_BUILD_TYPE=debug ..


- The sample rate of MP3 files must be less than or equal to 48000 Hz
