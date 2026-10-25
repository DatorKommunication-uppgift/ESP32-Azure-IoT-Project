import subprocess

HUB_NAME = "brook-nackademin-iothub"
GATEWAY_DEVICE = "raspberrypi-gateway"

print("Connecting to Azure IoT Hub...")
print("Waiting for sound and temperature measurements...")
print("Press Ctrl+C to stop.\n")

command = [
    r"C:\Program Files\Microsoft SDKs\Azure\CLI2\wbin\az.cmd",
    "iot",
    "hub",
    "monitor-events",
    "--hub-name", HUB_NAME,
    "--device-id", GATEWAY_DEVICE,
]

try:
    subprocess.run(command, check=True)
except KeyboardInterrupt:
    print("\nAzure consumer stopped.")
except FileNotFoundError:
    print("Azure CLI is not installed or not available in PATH.")
except subprocess.CalledProcessError as error:
    print("Azure monitoring failed:", error)