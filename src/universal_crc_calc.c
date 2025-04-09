#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>



typedef struct {
    uint8_t resolution;
    uint32_t poly;
    uint32_t init;
    bool refin;
    bool refout;
    uint32_t xorout;
} crc_calc_settings_t;

typedef struct {
    const uint8_t *name;
    const crc_calc_settings_t *crc_calc_settings;
    uint32_t check_value;   // for string "123456789"
} crc_test_t;


    
const crc_calc_settings_t crc_8 = {
    .resolution = 8,
    .poly = 0x7,
    .init = 0x0,
    .refin = false,
    .refout = false,
    .xorout = 0x0,
};

const crc_calc_settings_t crc_8_cdma2000 = {
    .resolution = 8,
    .poly = 0x9b,
    .init = 0xff,
    .refin = false,
    .refout = false,
    .xorout = 0x0,
};

const crc_calc_settings_t crc_8_darc = {
    .resolution = 8,
    .poly = 0x39,
    .init = 0x0,
    .refin = true,
    .refout = true,
    .xorout = 0x0,
};

const crc_calc_settings_t crc_8_dvb_s2 = {
    .resolution = 8,
    .poly = 0xd5,
    .init = 0x0,
    .refin = false,
    .refout = false,
    .xorout = 0x0,
};

const crc_calc_settings_t crc_8_ebu = {
    .resolution = 8,
    .poly = 0x1d,
    .init = 0xff,
    .refin = true,
    .refout = true,
    .xorout = 0x0,
};

const crc_calc_settings_t crc_8_i_code = {
    .resolution = 8,
    .poly = 0x1d,
    .init = 0xfd,
    .refin = false,
    .refout = false,
    .xorout = 0x0,
};

const crc_calc_settings_t crc_8_itu = {
    .resolution = 8,
    .poly = 0x7,
    .init = 0x0,
    .refin = false,
    .refout = false,
    .xorout = 0x55,
};

const crc_calc_settings_t crc_8_maxim = {
    .resolution = 8,
    .poly = 0x31,
    .init = 0x0,
    .refin = true,
    .refout = true,
    .xorout = 0x0,
};

const crc_calc_settings_t crc_8_rohc = {
    .resolution = 8,
    .poly = 0x7,
    .init = 0xff,
    .refin = true,
    .refout = true,
    .xorout = 0x0,
};

const crc_calc_settings_t crc_8_wcdma = {
    .resolution = 8,
    .poly = 0x9b,
    .init = 0x0,
    .refin = true,
    .refout = true,
    .xorout = 0x0,
};

const crc_calc_settings_t crc_16_arc = {
    .resolution = 16,
    .poly = 0x8005,
    .init = 0x0,
    .refin = true,
    .refout = true,
    .xorout = 0x0,
};

const crc_calc_settings_t crc_16_aug_ccitt = {
    .resolution = 16,
    .poly = 0x1021,
    .init = 0x1d0f,
    .refin = false,
    .refout = false,
    .xorout = 0x0,
};

const crc_calc_settings_t crc_16_buypass = {
    .resolution = 16,
    .poly = 0x8005,
    .init = 0x0,
    .refin = false,
    .refout = false,
    .xorout = 0x0,
};

const crc_calc_settings_t crc_16_ccitt_false = {
    .resolution = 16,
    .poly = 0x1021,
    .init = 0xffff,
    .refin = false,
    .refout = false,
    .xorout = 0x0,
};

const crc_calc_settings_t crc_16_cdma2000 = {
    .resolution = 16,
    .poly = 0xc867,
    .init = 0xffff,
    .refin = false,
    .refout = false,
    .xorout = 0x0,
};

const crc_calc_settings_t crc_16_dds_110 = {
    .resolution = 16,
    .poly = 0x8005,
    .init = 0x800d,
    .refin = false,
    .refout = false,
    .xorout = 0x0,
};

const crc_calc_settings_t crc_16_dect_r = {
    .resolution = 16,
    .poly = 0x589,
    .init = 0x0,
    .refin = false,
    .refout = false,
    .xorout = 0x1,
};

const crc_calc_settings_t crc_16_dect_x = {
    .resolution = 16,
    .poly = 0x589,
    .init = 0x0,
    .refin = false,
    .refout = false,
    .xorout = 0x0,
};

const crc_calc_settings_t crc_16_dnp = {
    .resolution = 16,
    .poly = 0x3d65,
    .init = 0x0,
    .refin = true,
    .refout = true,
    .xorout = 0xffff,
};

const crc_calc_settings_t crc_16_en_13757 = {
    .resolution = 16,
    .poly = 0x3d65,
    .init = 0x0,
    .refin = false,
    .refout = false,
    .xorout = 0xffff,
};

const crc_calc_settings_t crc_16_genibus = {
    .resolution = 16,
    .poly = 0x1021,
    .init = 0xffff,
    .refin = false,
    .refout = false,
    .xorout = 0xffff,
};

const crc_calc_settings_t crc_16_maxim = {
    .resolution = 16,
    .poly = 0x8005,
    .init = 0x0,
    .refin = true,
    .refout = true,
    .xorout = 0xffff,
};

const crc_calc_settings_t crc_16_mcrf4xx = {
    .resolution = 16,
    .poly = 0x1021,
    .init = 0xffff,
    .refin = true,
    .refout = true,
    .xorout = 0x0,
};

const crc_calc_settings_t crc_16_riello = {
    .resolution = 16,
    .poly = 0x1021,
    .init = 0xb2aa,
    .refin = true,
    .refout = true,
    .xorout = 0x0,
};

const crc_calc_settings_t crc_16_t10_dif = {
    .resolution = 16,
    .poly = 0x8bb7,
    .init = 0x0,
    .refin = false,
    .refout = false,
    .xorout = 0x0,
};

const crc_calc_settings_t crc_16_teledisk = {
    .resolution = 16,
    .poly = 0xa097,
    .init = 0x0,
    .refin = false,
    .refout = false,
    .xorout = 0x0,
};

const crc_calc_settings_t crc_16_tms37157 = {
    .resolution = 16,
    .poly = 0x1021,
    .init = 0x89ec,
    .refin = true,
    .refout = true,
    .xorout = 0x0,
};

const crc_calc_settings_t crc_16_usb = {
    .resolution = 16,
    .poly = 0x8005,
    .init = 0xffff,
    .refin = true,
    .refout = true,
    .xorout = 0xffff,
};

const crc_calc_settings_t crc_16_kermit = {
    .resolution = 16,
    .poly = 0x1021,
    .init = 0x0,
    .refin = true,
    .refout = true,
    .xorout = 0x0,
};

const crc_calc_settings_t crc_16_modbus = {
    .resolution = 16,
    .poly = 0x8005,
    .init = 0xffff,
    .refin = true,
    .refout = true,
    .xorout = 0x0,
};

const crc_calc_settings_t crc_16_x_25 = {
    .resolution = 16,
    .poly = 0x1021,
    .init = 0xffff,
    .refin = true,
    .refout = true,
    .xorout = 0xffff,
};

const crc_calc_settings_t crc_16_xmodem = {
    .resolution = 16,
    .poly = 0x1021,
    .init = 0x0,
    .refin = false,
    .refout = false,
    .xorout = 0x0,
};

const crc_calc_settings_t crc_32_zlib = {
    .resolution = 32,
    .poly = 0x4c11db7,
    .init = 0xffffffff,
    .refin = true,
    .refout = true,
    .xorout = 0xffffffff,
};

const crc_calc_settings_t crc_32_bzip2 = {
    .resolution = 32,
    .poly = 0x4c11db7,
    .init = 0xffffffff,
    .refin = false,
    .refout = false,
    .xorout = 0xffffffff,
};

const crc_calc_settings_t crc_32c = {
    .resolution = 32,
    .poly = 0x1edc6f41,
    .init = 0xffffffff,
    .refin = true,
    .refout = true,
    .xorout = 0xffffffff,
};

const crc_calc_settings_t crc_32d = {
    .resolution = 32,
    .poly = 0xa833982b,
    .init = 0xffffffff,
    .refin = true,
    .refout = true,
    .xorout = 0xffffffff,
};

const crc_calc_settings_t crc_32_mpeg_2 = {
    .resolution = 32,
    .poly = 0x4c11db7,
    .init = 0xffffffff,
    .refin = false,
    .refout = false,
    .xorout = 0x0,
};

const crc_calc_settings_t crc_32_posix = {
    .resolution = 32,
    .poly = 0x4c11db7,
    .init = 0x0,
    .refin = false,
    .refout = false,
    .xorout = 0xffffffff,
};

const crc_calc_settings_t crc_32q = {
    .resolution = 32,
    .poly = 0x814141ab,
    .init = 0x0,
    .refin = false,
    .refout = false,
    .xorout = 0x0,
};

const crc_calc_settings_t crc_32_jamcrc = {
    .resolution = 32,
    .poly = 0x4c11db7,
    .init = 0xffffffff,
    .refin = true,
    .refout = true,
    .xorout = 0x0,
};

const crc_calc_settings_t crc_32_xfer = {
    .resolution = 32,
    .poly = 0xaf,
    .init = 0x0,
    .refin = false,
    .refout = false,
    .xorout = 0x0,
};


const crc_test_t crc_tests[] = {
	{
        .name = "crc_8",
		.crc_calc_settings = &crc_8,
        .check = 0xf4
    },
    {
        .name = "crc_8_cdma2000",
		.crc_calc_settings = &crc_8_cdma2000,
        .check = 0xda
    },
    {
        .name = "crc_8_darc",
		.crc_calc_settings = &crc_8_darc,
        .check = 0x15
    },
    {
        .name = "crc_8_dvb_s2",
		.crc_calc_settings = &crc_8_dvb_s2,
        .check = 0xbc
    },
    {
        .name = "crc_8_ebu",
		.crc_calc_settings = &crc_8_ebu,
        .check = 0x97
    },
    {
        .name = "crc_8_i_code",
		.crc_calc_settings = &crc_8_i_code,
        .check = 0x7e
    },
    {
        .name = "crc_8_itu",
		.crc_calc_settings = &crc_8_itu,
        .check = 0xa1
    },
    {
        .name = "crc_8_maxim",
		.crc_calc_settings = &crc_8_maxim,
        .check = 0xa1
    },
    {
        .name = "crc_8_rohc",
		.crc_calc_settings = &crc_8_rohc,
        .check = 0xd0
    },
    {
        .name = "crc_8_wcdma",
		.crc_calc_settings = &crc_8_wcdma,
        .check = 0x25
    },
    {
        .name = "crc_16_arc",
		.crc_calc_settings = &crc_16_arc,
        .check = 0xbb3d
    },
    {
        .name = "crc_16_aug_ccitt",
		.crc_calc_settings = &crc_16_aug_ccitt,
        .check = 0xe5cc
    },
    {
        .name = "crc_16_buypass",
		.crc_calc_settings = &crc_16_buypass,
        .check = 0xfee8
    },
    {
        .name = "crc_16_ccitt_false",
		.crc_calc_settings = &crc_16_ccitt_false,
        .check = 0x29b1
    },
    {
        .name = "crc_16_cdma2000",
		.crc_calc_settings = &crc_16_cdma2000,
        .check = 0x4c06
    },
    {
        .name = "crc_16_dds_110",
		.crc_calc_settings = &crc_16_dds_110,
        .check = 0x9ecf
    },
    {
        .name = "crc_16_dect_r",
		.crc_calc_settings = &crc_16_dect_r,
        .check = 0x7e
    },
    {
        .name = "crc_16_dect_x",
		.crc_calc_settings = &crc_16_dect_x,
        .check = 0x7f
    },
    {
        .name = "crc_16_dnp",
		.crc_calc_settings = &crc_16_dnp,
        .check = 0xea82
    },
    {
        .name = "crc_16_en_13757",
		.crc_calc_settings = &crc_16_en_13757,
        .check = 0xc2b7
    },
    {
        .name = "crc_16_genibus",
		.crc_calc_settings = &crc_16_genibus,
        .check = 0xd64e
    },
    {
        .name = "crc_16_maxim",
		.crc_calc_settings = &crc_16_maxim,
        .check = 0x44c2
    },
    {
        .name = "crc_16_mcrf4xx",
		.crc_calc_settings = &crc_16_mcrf4xx,
        .check = 0x6f91
    },
    {
        .name = "crc_16_riello",
		.crc_calc_settings = &crc_16_riello,
        .check = 0x63d0
    },
    {
        .name = "crc_16_t10_dif",
		.crc_calc_settings = &crc_16_t10_dif,
        .check = 0xd0db
    },
    {
        .name = "crc_16_teledisk",
		.crc_calc_settings = &crc_16_teledisk,
        .check = 0xfb3
    },
    {
        .name = "crc_16_tms37157",
		.crc_calc_settings = &crc_16_tms37157,
        .check = 0x26b1
    },
    {
        .name = "crc_16_usb",
		.crc_calc_settings = &crc_16_usb,
        .check = 0xb4c8
    },
    {
        .name = "crc_16_kermit",
		.crc_calc_settings = &crc_16_kermit,
        .check = 0x2189
    },
    {
        .name = "crc_16_modbus",
		.crc_calc_settings = &crc_16_modbus,
        .check = 0x4b37
    },
    {
        .name = "crc_16_x_25",
		.crc_calc_settings = &crc_16_x_25,
        .check = 0x906e
    },
    {
        .name = "crc_16_xmodem",
		.crc_calc_settings = &crc_16_xmodem,
        .check = 0x31c3
    },
	{
        .name = "crc_32_zlib",
		.crc_calc_settings = &crc_32_zlib,
        .check = 0xcbf43926
    },
    {
        .name = "crc_32_bzip2",
		.crc_calc_settings = &crc_32_bzip2,
        .check = 0xfc891918
    },
    {
        .name = "crc_32c",
		.crc_calc_settings = &crc_32cffff,
        .check = 0xe3069283
    },
    {
        .name = "crc_32d",
		.crc_calc_settings = &crc_32dffff,
        .check = 0x87315576
    },
    {
        .name = "crc_32_mpeg_2",
		.crc_calc_settings = &crc_32_mpeg_2,
        .check = 0x376e6e7
    },
    {
        .name = "crc_32_posix",
		.crc_calc_settings = &crc_32_posix,
        .check = 0x765e7680
    },
    {
        .name = "crc_32q",
		.crc_calc_settings = &crc_32q,
        .check = 0x3010bf7f
    },
    {
        .name = "crc_32_jamcrc",
		.crc_calc_settings = &crc_32_jamcrc,
        .check = 0x340bc6d9
    },
    {
        .name = "crc_32_xfer",
		.crc_calc_settings = &crc_32_xfer,
        .check = 0xbd0be338
    }
}



void crc_create_table(uint8_t data_resolution, const crc_calc_settings_t *crc_settings, const void *crc_table, uint32_t *crc_table_size) {
    uint32_t crc_bit;
    uint32_t i;
    uint32_t poly;
    uint32_t table_size;
    uint32_t table_index;
    uint32_t table_value;


    if (crc_settings->refout) {
        // Poly mirrored
        poly = 0;
        for (i = 0; i < crc_settings->resolution; i++) {
            poly = poly << 1;
            poly |= (crc_settings->poly >> i) & 0x1;
        }
    }
    else {
        poly = crc_settings->poly;
    }


    table_size = (1 << data_resolution);


    if (crc_settings->refin) {
        for (table_index = 0; table_index < table_size; table_index++) {
            table_value = table_index;
            for (crc_bit = 0; crc_bit < data_resolution; crc_bit++) {
                if (table_value & 0x01) {
                    table_value = (table_value >> 1) ^ poly;
                }
                else {
                    table_value = (table_value >> 1);
                }
            }

            if (crc_settings->resolution == 8) ((uint8_t*)crc_table)[table_index] = (uint8_t)table_value;
            else if (crc_settings->resolution == 16) ((uint16_t*)crc_table)[table_index] = (uint16_t)table_value;
            else if (crc_settings->resolution == 32) ((uint32_t*)crc_table)[table_index] = (uint32_t)table_value;
        }
    }
    else {
        for (table_index = 0; table_index < table_size; table_index++) {
            if (data_resolution == 8) table_value = table_index << (crc_settings->resolution - 8);
            else if (data_resolution == 16) table_value = table_index << (crc_settings->resolution - 16);
            else if (data_resolution == 32) table_value = table_index;

            for (crc_bit = 0; crc_bit < data_resolution; crc_bit++) {
                if (table_value & (1 << (crc_settings->resolution - 1))) {
                    table_value = (table_value << 1) ^ poly;
                }
                else {
                    table_value = (table_value << 1);
                }
            }

            if (crc_settings->resolution == 8) ((uint8_t*)crc_table)[table_index] = (uint8_t)table_value;
            else if (crc_settings->resolution == 16) ((uint16_t*)crc_table)[table_index] = (uint16_t)table_value;
            else if (crc_settings->resolution == 32) ((uint32_t*)crc_table)[table_index] = (uint32_t)table_value;
        }
    }

    if (crc_table_size != NULL) *crc_table_size = table_size;
}


uint32_t crc_continue_clac_by_table(uint32_t init, const void *data, uint8_t data_resolution, uint32_t data_qty, const crc_calc_settings_t *crc_settings, const void *crc_table) {
    uint32_t crc;
    uint32_t data_index;
    uint32_t data_conv;
    uint32_t i;
    uint32_t init_old;
    uint32_t table_index;


    if (crc_settings->refout) {
        // Init mirrored
        init_old = init;
        init = 0;
        for (i = 0; i < crc_settings->resolution; i++) {
            init = init << 1;
            init |= (init_old >> i) & 0x1;
        }
    }


    crc = init;
    if (crc_settings->refin) {
        for (data_index = 0; data_index < data_qty; data_index++) {
            if (data_resolution == 8) data_conv = ((uint8_t*)data)[data_index];
            else if (data_resolution == 16) data_conv = ((uint16_t*)data)[data_index];
            else if (data_resolution == 32) data_conv = ((uint32_t*)data)[data_index];

            table_index = crc ^ data_conv;
            table_index &= ((1 << data_resolution) - 1);
            crc >>= data_resolution;

            if (crc_settings->resolution == 8) crc ^= ((uint8_t*)crc_table)[table_index];
            else if (crc_settings->resolution == 16) crc ^= ((uint16_t*)crc_table)[table_index];
            else if (crc_settings->resolution == 32) crc ^= ((uint32_t*)crc_table)[table_index];
        }
    }
    else {
        for (data_index = 0; data_index < data_qty; data_index++) {
            if (data_resolution == 8) data_conv = ((uint8_t*)data)[data_index];
            else if (data_resolution == 16) data_conv = ((uint16_t*)data)[data_index];
            else if (data_resolution == 32) data_conv = ((uint32_t*)data)[data_index];

            table_index = (crc >> (crc_settings->resolution - data_resolution)) ^ data_conv;
            table_index &= ((1 << data_resolution) - 1);
            crc <<= data_resolution;

            if (crc_settings->resolution == 8) crc ^= ((uint8_t*)crc_table)[table_index];
            else if (crc_settings->resolution == 16) crc ^= ((uint16_t*)crc_table)[table_index];
            else if (crc_settings->resolution == 32) crc ^= ((uint32_t*)crc_table)[table_index];
        }
    }

    crc ^= crc_settings->xorout;
    return crc;
}


uint32_t crc_continue_clac(uint32_t init, const void *data, uint8_t data_resolution, uint32_t data_qty, const crc_calc_settings_t *crc_settings) {
    uint32_t crc;
    uint32_t data_index, crc_bit;
    uint32_t data_conv;
    uint32_t i;
    uint32_t poly;
    uint32_t init_old;


    if (crc_settings->refout) {
        // Poly mirrored
        poly = 0;
        for (i = 0; i < crc_settings->resolution; i++) {
            poly = poly << 1;
            poly |= (crc_settings->poly >> i) & 0x1;
        }

        // Init mirrored
        init_old = init;
        init = 0;
        for (i = 0; i < crc_settings->resolution; i++) {
            init = init << 1;
            init |= (init_old >> i) & 0x1;
        }
    }
    else {
        poly = crc_settings->poly;
    }


    crc = init;
    if (crc_settings->refin) {
        for (data_index = 0; data_index < data_qty; data_index++) {

            if (data_resolution == 8) data_conv = ((uint8_t*)data)[data_index];
            else if (data_resolution == 16) data_conv = ((uint16_t*)data)[data_index];
            else if (data_resolution == 32) data_conv = ((uint32_t*)data)[data_index];

            crc ^= data_conv;
            for (crc_bit = 0; crc_bit < data_resolution; crc_bit++) {
                if (crc & 0x01) {
                    crc = (crc >> 1) ^ poly;
                }
                else {
                    crc = (crc >> 1);
                }
            }
        }
    }
    else {
        for (data_index = 0; data_index < data_qty; data_index++) {
            if (data_resolution == 8) data_conv = ((uint8_t*)data)[data_index] << (crc_settings->resolution - 8);
            else if (data_resolution == 16) data_conv = ((uint16_t*)data)[data_index] << (crc_settings->resolution - 16);
            else if (data_resolution == 32) data_conv = ((uint32_t*)data)[data_index];

            crc ^= data_conv;
            for (crc_bit = 0; crc_bit < data_resolution; crc_bit++) {
                if (crc & (1 << (crc_settings->resolution - 1))) {
                    crc = (crc << 1) ^ poly;
                }
                else {
                    crc = (crc << 1);
                }
            }
        }
    }

    crc ^= crc_settings->xorout;
    return crc;
}



void check_test(int32_t v1, int32_t v2) {
    if (v1 != v2) printf("ERR!!   %d != %d\r\n", v1, v2);
    else printf("ok\r\n");
}

void check_utest(uint32_t v1, uint32_t v2) {
    if (v1 != v2) printf("ERR!!   %d != %d\r\n", v1, v2);
    else printf("ok\r\n");
}



int main() {
    uint8_t crc_test_data[] = "123456789";
    uint32_t crc, i;
    uint8_t data_resolution;
    uint8_t crc8_table[256];
    uint16_t crc16_table[256];
    uint32_t crc32_table[256];


    printf("Table calc tests, input data u8:\r\n")
    data_resolution = 8;
    for (i = 0; i < (sizeof(crc_tests) / sizeof(crc_test_t)); i++) {
        if (crc_tests[i].crc_calc_settings->resolution == 8) {
            crc_create_table(data_resolution, crc_tests[i].crc_calc_settings, (void*)crc8_table, NULL);
            crc = (uint8_t)crc_continue_clac_by_table(crc_tests[i].crc_calc_settings->init, crc_test_data, data_resolution, 9, crc_tests[i].crc_calc_settings, (void*)crc8_table);
        }
        if (crc_tests[i].crc_calc_settings->resolution == 16) {
            crc_create_table(data_resolution, crc_tests[i].crc_calc_settings, (void*)crc16_table, NULL);
            crc = (uint16_t)crc_continue_clac_by_table(crc_tests[i].crc_calc_settings->init, crc_test_data, data_resolution, 9, crc_tests[i].crc_calc_settings, (void*)crc16_table);
        }
        if (crc_tests[i].crc_calc_settings->resolution == 32) {
            crc_create_table(data_resolution, crc_tests[i].crc_calc_settings, (void*)crc32_table, NULL);
            crc = (uint32_t)crc_continue_clac_by_table(crc_tests[i].crc_calc_settings->init, crc_test_data, data_resolution, 9, crc_tests[i].crc_calc_settings, (void*)crc32_table);
        }
        printf("%s:  ", crc_tests[i].crc_calc_settings->name);
        check_utest(crc, crc_tests[i].check);
    }

    printf("-------------------------------------\r\n")
    printf("Bit calc tests, input data u8:\r\n")
    data_resolution = 8;
    for (i = 0; i < (sizeof(crc_tests) / sizeof(crc_test_t)); i++) {
        if (crc_tests[i].crc_calc_settings->resolution == 8) {
            crc = (uint8_t)crc_continue_clac(crc_tests[i].crc_calc_settings->init, crc_test_data, data_resolution, 9, crc_tests[i].crc_calc_settings);
        }
        if (crc_tests[i].crc_calc_settings->resolution == 16) {
            crc = (uint16_t)crc_continue_clac(crc_tests[i].crc_calc_settings->init, crc_test_data, data_resolution, 9, crc_tests[i].crc_calc_settings);
        }
        if (crc_tests[i].crc_calc_settings->resolution == 32) {
            crc = (uint32_t)crc_continue_clac(crc_tests[i].crc_calc_settings->init, crc_test_data, data_resolution, 9, crc_tests[i].crc_calc_settings);
        }
        printf("%s:  ", crc_tests[i].crc_calc_settings->name);
        check_utest(crc, crc_tests[i].check);
    }

    return 0;
}

