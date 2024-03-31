from ConfigParser import WirelessNodeTypes, MessageHeaders

def construct_message(target_device: str, message: str):
    """
        Construct a message to be sent to the target device
    """
    return f"{MessageHeaders.START.value}{MessageHeaders.TARGET_DEVICE.value}{target_device}{MessageHeaders.BODY.value}{message}{MessageHeaders.END.value}"

