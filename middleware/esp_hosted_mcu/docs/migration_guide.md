# Migration Guide

This guide documents changes in ESP-Hosted that users must be aware of when migrating from older versions of ESP-Hosted.

## Migrating to V2.5.2

### Bluetooth Controller on Co-processor Now Disabled by Default

Before V2.5.2, the Bluetooth Controller on the co-processor was initialized and enabled by default when the co-processor starts. Now, the Controller is left in a disabled state upon start-up.

**Reason for Change.**

This change is to allow users to change the Bluetooth MAC Address of the co-processor from the default MAC address, if required. This can only be done before the Bluetooth Controller is initialized and enabled.

**Changes required on Host.**

Host must enable the Bluetooth Controller on the co-processor, before starting the Bluetooth Stack on the Host. To do this:

1. Call `esp_hosted_connect_to_slave()` to ensure there is a ESP-Hosted connection to the slave.
2. **[Optional]** Set the BT Mac address using `esp_hosted_iface_mac_addr_set`
3. Initialise the BT Controller using `esp_hosted_bt_controller_init`
4. Enable the BT Controller using `esp_hosted_bt_controller_enable`

See [Initializing the Bluetooth Controller](bluetooth_design.md#31-initializing-the-bluetooth-controller) for more details.

You can now start the Host Bluetooth Stack and use Bluetooth as per normal.

All ESP-Hosted Bluetooth Host examples (NimBLE and BlueDroid) have been updated with this change.

For an example on how to change the BT Mac Address before starting the Bluetooth Controller, see:

- [BT Controller example](../examples/host_bt_controller_mac_addr/)
