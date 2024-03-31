from UofA_BAJA_2023_2024_common.enums import MessageHeaders

def construct_message(target_device: str, message: str):
    """
        Construct a message to be sent to the target device
    """
    return f"{MessageHeaders.START}{MessageHeaders.TARGET_DEVICE}{target_device}{MessageHeaders.BODY}{message}{MessageHeaders.END}"

