import serial
import time


port = "COM10"
baudrate = 115200


profiles = [
    {
        "name": "drying_water",
        "stages": [
            {
                "temperature_c" : 60,
                "duration_s": 240,
                "fun_period_s": 60,
                "fun_duty_cycle_pct": 50
            },
            {
                "temperature_c" : 0,
                "duration_s": 0,
                "fun_period_s": 0,
                "fun_duty_cycle_pct": 0
            },
            {
                "temperature_c" : 0,
                "duration_s": 0,
                "fun_period_s": 0,
                "fun_duty_cycle_pct": 0
            }
        ]
    },
    {
        "name": "mask_begin",
        "stages": [
            {
                "temperature_c" : 70,
                "duration_s": 300,
                "fun_period_s": 0,
                "fun_duty_cycle_pct": 0
            },
            {
                "temperature_c" : 70,
                "duration_s": 600,
                "fun_period_s": 120,
                "fun_duty_cycle_pct": 10
            },
            {
                "temperature_c" : 0,
                "duration_s": 0,
                "fun_period_s": 0,
                "fun_duty_cycle_pct": 0
            }
        ]
    },
    {
        "name": "mask_end",
        "stages": [
            {
                "temperature_c" : 140,
                "duration_s": 600,
                "fun_period_s": 0,
                "fun_duty_cycle_pct": 0
            },
            {
                "temperature_c" : 140,
                "duration_s": 600,
                "fun_period_s": 120,
                "fun_duty_cycle_pct": 10
            },
            {
                "temperature_c" : 140,
                "duration_s": 600,
                "fun_period_s": 0,
                "fun_duty_cycle_pct": 0
            }
        ]
    },
    {
        "name": "",
        "stages": [
            {
                "temperature_c" : 0,
                "duration_s": 0,
                "fun_period_s": 0,
                "fun_duty_cycle_pct": 0
            },
            {
                "temperature_c" : 0,
                "duration_s": 0,
                "fun_period_s": 0,
                "fun_duty_cycle_pct": 0
            },
            {
                "temperature_c" : 0,
                "duration_s": 0,
                "fun_period_s": 0,
                "fun_duty_cycle_pct": 0
            }
        ]
    },
]



def wait_prompt():
    ser.read_until(b">", 100)


def write_reg_u16(address:int, data:int):
    message = "wr " + str(address) + " " + str(data) + "\x0D"
    ser.write(message.encode())
    wait_prompt()


def write_reg_u32(address:int, data:int):
    reg_lo = data & 0x0000FFFF
    reg_hi = data >> 8
    write_reg_u16(address, reg_lo)
    write_reg_u16((address + 1), reg_hi)


def write_reg_string(address:int, data:str):
    name_symb_in_reg = 0
    regs_values_qty = 0
    regs_values:int = []

    for symb in data:
        if name_symb_in_reg == 0:
            regs_values.append(0)
            regs_values_qty += 1
            regs_values[regs_values_qty - 1] = ord(symb)
        else:
            regs_values[regs_values_qty - 1] |= ord(symb) << 8
        name_symb_in_reg += 1
        if name_symb_in_reg >= 2:
            name_symb_in_reg = 0
    if name_symb_in_reg == 1:
        regs_values[regs_values_qty - 1] |= '\0' << 8

    for reg_value in regs_values:
        write_reg_u16(address, reg_value)
        address += 1




ser = serial.Serial(port, baudrate=baudrate)
ser.timeout = 2
print("Start\r\n")
ser.write(b"\r")
wait_prompt()
print("Flash erase...\r\n")
ser.write(b"wr 7 2\r")   # Flash erase
wait_prompt()
time.sleep(2.0)
reg_addr = 8
print("Write profiles...\r\n")
for profile in profiles:
    name = profile["name"]
    while len(name) < 18:
        name += '\0'
    write_reg_string(reg_addr, name)
    reg_addr += 9

    for stage in profile["stages"]:
        write_reg_u16(reg_addr, stage["temperature_c"])
        reg_addr += 1
        write_reg_u32(reg_addr, stage["duration_s"])
        reg_addr += 2
        write_reg_u32(reg_addr, stage["fun_period_s"])
        reg_addr += 2
        write_reg_u16(reg_addr, stage["fun_duty_cycle_pct"])
        reg_addr += 1


time.sleep(2.0)
print("Update CRC...\r\n")
ser.write(b"wr 7 3\r")   # Update CRC
time.sleep(2.0)
print("Reboot...\r\n")
ser.write(b"wr 7 1\r")   # Reboot

print("Done!\r\n")
ser.close()




    