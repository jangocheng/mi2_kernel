/* Copyright (c) 2012, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include "msm_sensor.h"
#define SENSOR_NAME "imx135"
#define PLATFORM_DRIVER_NAME "msm_camera_imx135"
#define imx135_obj imx135_##obj

DEFINE_MUTEX(imx135_mut);

#define SHORT_SHUTTER_WORD_ADDR		0x0230
#define SHORT_GAIN_BYTE_ADDR		0x0233
#define ABS_GAIN_R_WORD_ADDR		0x0714
#define ABS_GAIN_B_WORD_ADDR		0x0716
#define LSC_ENABLE_BYTE_ADDR		0x0700
#define EN_LSC_BYTE_ADDR			0x4500
#define RAM_SEL_TOGGLE_BYTE_ADDR	0x3A63
#define LSC_TABLE_START_ADDR		0x4800
#define TC_OUT_NOISE_REGADDR        0x4452
#define TC_OUT_MID_REGADDR          0x4454
#define TC_NOISE_BRATE_REGADDR      0x4458
#define TC_MID_BRATE_REGADDR        0x4459
#define TC_SWITCH_REGADDR           0x446C
#define DBG_AVE_H_REGADDR           0x4470
#define DBG_AVE_L_REGADDR           0x4471
#define DBG_MIN_H_REGADDR           0x4472
#define DBG_MIN_L_REGADDR           0x4473
#define DBG_MAX_H_REGADDR           0x4474
#define DBG_MAX_L_REGADDR           0X4475
#define DBG_SEL                     0x4476
#define LSC_TABLE_LEN_BYTES			504
#define IMX135_TC_SWITCH_ENABLE 0
static uint16_t luma_delta_hys;

static struct msm_sensor_ctrl_t imx135_s_ctrl;

static struct msm_camera_i2c_reg_conf imx135_start_settings[] = {
	{0x0100, 0x01},
};

static struct msm_camera_i2c_reg_conf imx135_stop_settings[] = {
	{0x0100, 0x00},
};

static struct msm_camera_i2c_reg_conf imx135_groupon_settings[] = {
	{0x104, 0x01},
};

static struct msm_camera_i2c_reg_conf imx135_groupoff_settings[] = {
	{0x104, 0x00},
};

static struct msm_camera_i2c_reg_conf imx135_recommend_settings[] = {
	{0x0101, 0x00},
	{0x0105, 0x01},
	{0x0110, 0x00},
	{0x0220, 0x01},
	{0x3302, 0x11},
	{0x3833, 0x20},
	{0x3893, 0x00},
	{0x3906, 0x08},
	{0x3907, 0x01},
	{0x391B, 0x01},
	{0x3C09, 0x01},
	{0x600A, 0x00},
	{0x3008, 0xB0},
	{0x320A, 0x01},
	{0x320D, 0x10},
	{0x3216, 0x2E},
	{0x322C, 0x02},
	{0x3409, 0x0C},
	{0x340C, 0x2D},
	{0x3411, 0x39},
	{0x3414, 0x1E},
	{0x3427, 0x04},
	{0x3480, 0x1E},
	{0x3484, 0x1E},
	{0x3488, 0x1E},
	{0x348C, 0x1E},
	{0x3490, 0x1E},
	{0x3494, 0x1E},
	{0x3511, 0x8F},
	{0x364F, 0x2D},
	{0x380A, 0x00},
	{0x380B, 0x00},
	{0x4103, 0x00},
	{0x4243, 0x9A},
	{0x4330, 0x01},
	{0x4331, 0x90},
	{0x4332, 0x02},
	{0x4333, 0x58},
	{0x4334, 0x03},
	{0x4335, 0x20},
	{0x4336, 0x03},
	{0x4337, 0x84},
	{0x433C, 0x01},
	{0x4340, 0x02},
	{0x4341, 0x58},
	{0x4342, 0x03},
	{0x4343, 0x52},
	{0x4364, 0x0B},
	{0x4368, 0x00},
	{0x4369, 0x0F},
	{0x436A, 0x03},
	{0x436B, 0xA8},
	{0x436C, 0x00},
	{0x436D, 0x00},
	{0x436E, 0x00},
	{0x436F, 0x06},
	{0x4281, 0x21},
	{0x4282, 0x18},
	{0x4283, 0x04},
	{0x4284, 0x08},
	{0x4287, 0x7F},
	{0x4288, 0x08},
	{0x428B, 0x7F},
	{0x428C, 0x08},
	{0x428F, 0x7F},
	{0x4297, 0x00},
	{0x4298, 0x7E},
	{0x4299, 0x7E},
	{0x429A, 0x7E},
	{0x42A4, 0xFB},
	{0x42A5, 0x7E},
	{0x42A6, 0xDF},
	{0x42A7, 0xB7},
	{0x42AF, 0x03},
	{0x4207, 0x03},
	{0x4216, 0x08},
	{0x4217, 0x08},
	{0x4218, 0x00},
	{0x421B, 0x20},
	{0x421F, 0x04},
	{0x4222, 0x02},
	{0x4223, 0x22},
	{0x422E, 0x54},
	{0x422F, 0xFB},
	{0x4230, 0xFF},
	{0x4231, 0xFE},
	{0x4232, 0xFF},
	{0x4235, 0x58},
	{0x4236, 0xF7},
	{0x4237, 0xFD},
	{0x4239, 0x4E},
	{0x423A, 0xFC},
	{0x423B, 0xFD},
	{0x4300, 0x00},
	{0x4316, 0x12},
	{0x4317, 0x22},
	{0x4318, 0x00},
	{0x4319, 0x00},
	{0x431A, 0x00},
	{0x4324, 0x03},
	{0x4325, 0x20},
	{0x4326, 0x03},
	{0x4327, 0x84},
	{0x4328, 0x03},
	{0x4329, 0x20},
	{0x432A, 0x03},
	{0x432B, 0x20},
	{0x432C, 0x01},
	{0x432D, 0x01},
	{0x4338, 0x02},
	{0x4339, 0x00},
	{0x433A, 0x00},
	{0x433B, 0x02},
	{0x435A, 0x03},
	{0x435B, 0x84},
	{0x435E, 0x01},
	{0x435F, 0xFF},
	{0x4360, 0x01},
	{0x4361, 0xF4},
	{0x4362, 0x03},
	{0x4363, 0x84},
	{0x437B, 0x01},
	{0x4401, 0x03},
	{0x4402, 0xFF},
	{0x4404, 0x13},
	{0x4405, 0x26},
	{0x4406, 0x07},
	{0x4408, 0x20},
	{0x4409, 0xE5},
	{0x440A, 0xFB},
	{0x440C, 0xF6},
	{0x440D, 0xEA},
	{0x440E, 0x20},
	{0x4410, 0x00},
	{0x4411, 0x00},
	{0x4412, 0x3F},
	{0x4413, 0xFF},
	{0x4414, 0x1F},
	{0x4415, 0xFF},
	{0x4416, 0x20},
	{0x4417, 0x00},
	{0x4418, 0x1F},
	{0x4419, 0xFF},
	{0x441A, 0x20},
	{0x441B, 0x00},
	{0x441D, 0x40},
	{0x441E, 0x1E},
	{0x441F, 0x38},
	{0x4420, 0x01},
	{0x4444, 0x00},
	{0x4445, 0x00},
	{0x4446, 0x1D},
	{0x4447, 0xF9},
	{0x4452, 0x00},
	{0x4453, 0xA0},
	{0x4454, 0x08},
	{0x4455, 0x00},
	{0x4456, 0x0F},
	{0x4457, 0xFF},
	{0x4458, 0x18},
	{0x4459, 0x18},
	{0x445A, 0x3F},
	{0x445B, 0x3A},
	{0x445C, 0x00},
	{0x445D, 0x28},
	{0x445E, 0x01},
	{0x445F, 0x90},
	{0x4460, 0x00},
	{0x4461, 0x60},
	{0x4462, 0x00},
	{0x4463, 0x00},
	{0x4464, 0x00},
	{0x4465, 0x00},
	{0x446C, 0x00},
	{0x446D, 0x00},
	{0x446E, 0x00},
	{0x452A, 0x02},
	{0x0712, 0x01},
	{0x0713, 0x00},
	{0x0714, 0x01},
	{0x0715, 0x00},
	{0x0716, 0x01},
	{0x0717, 0x00},
	{0x0718, 0x01},
	{0x0719, 0x00},
	{0x4500, 0x1F},
};

static struct msm_camera_i2c_reg_conf imx135_60fps_settings[] = {
	{0x011E, 0x18},
	{0x011F, 0x00},
	{0x0301, 0x05},
	{0x0303, 0x01},
	{0x0305, 0x03},
	{0x0309, 0x05},
	{0x030B, 0x02},
	{0x030C, 0x00},
	{0x030D, 0x50},
	{0x030E, 0x01},
	{0x3A06, 0x12},
	{0x0108, 0x03},
	{0x0112, 0x0A},
	{0x0113, 0x0A},
	{0x0381, 0x01},
	{0x0383, 0x01},
	{0x0385, 0x01},
	{0x0387, 0x01},
	{0x0390, 0x01},
	{0x0391, 0x22},
	{0x0392, 0x00},
	{0x0401, 0x02},
	{0x0404, 0x00},
	{0x0405, 0x19},
	{0x4082, 0x00},
	{0x4083, 0x00},
	{0x7006, 0x04},
	{0x0700, 0x00},
	{0x3A63, 0x00},
	{0x4100, 0xF8},
	{0x4203, 0xFF},
	{0x4344, 0x00},
	{0x441C, 0x01},
	{0x0340, 0x04},
	{0x0341, 0x9A},
	{0x0342, 0x11},
	{0x0343, 0xDC},
	{0x0344, 0x00},
	{0x0345, 0x36},
	{0x0346, 0x01},
	{0x0347, 0x9C},
	{0x0348, 0x10},
	{0x0349, 0x39},
	{0x034A, 0x0A},
	{0x034B, 0x97},
	{0x034C, 0x05},
	{0x034D, 0x20},
	{0x034E, 0x02},
	{0x034F, 0xE0},
	{0x0350, 0x00},
	{0x0351, 0x00},
	{0x0352, 0x00},
	{0x0353, 0x00},
	{0x0354, 0x08},
	{0x0355, 0x02},
	{0x0356, 0x04},
	{0x0357, 0x7E},
	{0x301D, 0x30},
	{0x3310, 0x05},
	{0x3311, 0x20},
	{0x3312, 0x02},
	{0x3313, 0xE0},
	{0x331C, 0x02},
	{0x331D, 0x1C},
	{0x4084, 0x05},
	{0x4085, 0x20},
	{0x4086, 0x02},
	{0x4087, 0xE0},
	{0x4400, 0x00},
	{0x0830, 0x67},
	{0x0831, 0x1F},
	{0x0832, 0x47},
	{0x0833, 0x1F},
	{0x0834, 0x1F},
	{0x0835, 0x17},
	{0x0836, 0x77},
	{0x0837, 0x27},
	{0x0839, 0x1F},
	{0x083A, 0x17},
	{0x083B, 0x02},
	{0x0202, 0x04},
	{0x0203, 0x96},
	{0x0205, 0x00},
	{0x020E, 0x01},
	{0x020F, 0x00},
	{0x0210, 0x01},
	{0x0211, 0x00},
	{0x0212, 0x01},
	{0x0213, 0x00},
	{0x0214, 0x01},
	{0x0215, 0x00},
	{0x0230, 0x00},
	{0x0231, 0x00},
	{0x0233, 0x00},
	{0x0234, 0x00},
	{0x0235, 0x40},
	{0x0238, 0x00},
	{0x0239, 0x04},
	{0x023B, 0x00},
	{0x023C, 0x01},
	{0x33B0, 0x04},
	{0x33B1, 0x00},
	{0x33B3, 0x00},
	{0x33B4, 0x01},
	{0x3800, 0x00},
};

static struct msm_camera_i2c_reg_conf imx135_90fps_settings[] = {
	{0x011E, 0x18},
	{0x011F, 0x00},
	{0x0301, 0x05},
	{0x0303, 0x01},
	{0x0305, 0x03},
	{0x0309, 0x05},
	{0x030B, 0x02},
	{0x030C, 0x00},
	{0x030D, 0x64},
	{0x030E, 0x01},
	{0x3A06, 0x12},
	{0x0108, 0x03},
	{0x0112, 0x0A},
	{0x0113, 0x0A},
	{0x0381, 0x01},
	{0x0383, 0x01},
	{0x0385, 0x01},
	{0x0387, 0x01},
	{0x0390, 0x01},
	{0x0391, 0x22},
	{0x0392, 0x00},
	{0x0401, 0x00},
	{0x0404, 0x00},
	{0x0405, 0x10},
	{0x4082, 0x01},
	{0x4083, 0x01},
	{0x7006, 0x04},
	{0x0700, 0x00},
	{0x3A63, 0x00},
	{0x4100, 0xF8},
	{0x4203, 0xFF},
	{0x4344, 0x00},
	{0x441C, 0x01},
	{0x0340, 0x03},
	{0x0341, 0x08},
	{0x0342, 0x11},
	{0x0343, 0xDC},
	{0x0344, 0x03},
	{0x0345, 0x18},
	{0x0346, 0x03},
	{0x0347, 0x38},
	{0x0348, 0x0D},
	{0x0349, 0x57},
	{0x034A, 0x08},
	{0x034B, 0xF7},
	{0x034C, 0x05},
	{0x034D, 0x20},
	{0x034E, 0x02},
	{0x034F, 0xE0},
	{0x0350, 0x00},
	{0x0351, 0x00},
	{0x0352, 0x00},
	{0x0353, 0x00},
	{0x0354, 0x05},
	{0x0355, 0x20},
	{0x0356, 0x02},
	{0x0357, 0xE0},
	{0x301D, 0x30},
	{0x3310, 0x05},
	{0x3311, 0x20},
	{0x3312, 0x02},
	{0x3313, 0xE0},
	{0x331C, 0x00},
	{0x331D, 0x10},
	{0x4084, 0x00},
	{0x4085, 0x00},
	{0x4086, 0x00},
	{0x4087, 0x00},
	{0x4400, 0x00},
	{0x0830, 0x67},
	{0x0831, 0x1F},
	{0x0832, 0x47},
	{0x0833, 0x1F},
	{0x0834, 0x1F},
	{0x0835, 0x17},
	{0x0836, 0x77},
	{0x0837, 0x27},
	{0x0839, 0x1F},
	{0x083A, 0x17},
	{0x083B, 0x02},
	{0x0202, 0x03},
	{0x0203, 0x04},
	{0x0205, 0x00},
	{0x020E, 0x01},
	{0x020F, 0x00},
	{0x0210, 0x01},
	{0x0211, 0x00},
	{0x0212, 0x01},
	{0x0213, 0x00},
	{0x0214, 0x01},
	{0x0215, 0x00},
	{0x0230, 0x00},
	{0x0231, 0x00},
	{0x0233, 0x00},
	{0x0234, 0x00},
	{0x0235, 0x40},
	{0x0238, 0x00},
	{0x0239, 0x04},
	{0x023B, 0x00},
	{0x023C, 0x01},
	{0x33B0, 0x04},
	{0x33B1, 0x00},
	{0x33B3, 0x00},
	{0x33B4, 0x01},
	{0x3800, 0x00},
};

static struct msm_camera_i2c_reg_conf imx135_LSCTable_settings[] = {
	{0x4800, 0x01},
	{0x4801, 0xc2},
	{0x4802, 0x01},
	{0x4803, 0x8c},
	{0x4804, 0x01},
	{0x4805, 0xad},
	{0x4806, 0x01},
	{0x4807, 0x7b},
	{0x4808, 0x01},
	{0x4809, 0x8e},
	{0x480a, 0x01},
	{0x480b, 0x6e},
	{0x480c, 0x01},
	{0x480d, 0x72},
	{0x480e, 0x01},
	{0x480f, 0x5a},
	{0x4810, 0x01},
	{0x4811, 0x63},
	{0x4812, 0x01},
	{0x4813, 0x58},
	{0x4814, 0x01},
	{0x4815, 0x6e},
	{0x4816, 0x01},
	{0x4817, 0x58},
	{0x4818, 0x01},
	{0x4819, 0x80},
	{0x481a, 0x01},
	{0x481b, 0x67},
	{0x481c, 0x01},
	{0x481d, 0x98},
	{0x481e, 0x01},
	{0x481f, 0x74},
	{0x4820, 0x01},
	{0x4821, 0x9e},
	{0x4822, 0x01},
	{0x4823, 0x75},
	{0x4824, 0x01},
	{0x4825, 0xad},
	{0x4826, 0x01},
	{0x4827, 0x83},
	{0x4828, 0x01},
	{0x4829, 0x8f},
	{0x482a, 0x01},
	{0x482b, 0x6e},
	{0x482c, 0x01},
	{0x482d, 0x68},
	{0x482e, 0x01},
	{0x482f, 0x51},
	{0x4830, 0x01},
	{0x4831, 0x45},
	{0x4832, 0x01},
	{0x4833, 0x34},
	{0x4834, 0x01},
	{0x4835, 0x37},
	{0x4836, 0x01},
	{0x4837, 0x2b},
	{0x4838, 0x01},
	{0x4839, 0x3d},
	{0x483a, 0x01},
	{0x483b, 0x33},
	{0x483c, 0x01},
	{0x483d, 0x59},
	{0x483e, 0x01},
	{0x483f, 0x48},
	{0x4840, 0x01},
	{0x4841, 0x83},
	{0x4842, 0x01},
	{0x4843, 0x64},
	{0x4844, 0x01},
	{0x4845, 0x94},
	{0x4846, 0x01},
	{0x4847, 0x75},
	{0x4848, 0x01},
	{0x4849, 0xa3},
	{0x484a, 0x01},
	{0x484b, 0x7f},
	{0x484c, 0x01},
	{0x484d, 0x82},
	{0x484e, 0x01},
	{0x484f, 0x64},
	{0x4850, 0x01},
	{0x4851, 0x4c},
	{0x4852, 0x01},
	{0x4853, 0x38},
	{0x4854, 0x01},
	{0x4855, 0x1e},
	{0x4856, 0x01},
	{0x4857, 0x14},
	{0x4858, 0x01},
	{0x4859, 0x0e},
	{0x485a, 0x01},
	{0x485b, 0x08},
	{0x485c, 0x01},
	{0x485d, 0x17},
	{0x485e, 0x01},
	{0x485f, 0x13},
	{0x4860, 0x01},
	{0x4861, 0x3f},
	{0x4862, 0x01},
	{0x4863, 0x32},
	{0x4864, 0x01},
	{0x4865, 0x6e},
	{0x4866, 0x01},
	{0x4867, 0x55},
	{0x4868, 0x01},
	{0x4869, 0x8e},
	{0x486a, 0x01},
	{0x486b, 0x71},
	{0x486c, 0x01},
	{0x486d, 0xa8},
	{0x486e, 0x01},
	{0x486f, 0x85},
	{0x4870, 0x01},
	{0x4871, 0x7e},
	{0x4872, 0x01},
	{0x4873, 0x62},
	{0x4874, 0x01},
	{0x4875, 0x44},
	{0x4876, 0x01},
	{0x4877, 0x35},
	{0x4878, 0x01},
	{0x4879, 0x14},
	{0x487a, 0x01},
	{0x487b, 0x0d},
	{0x487c, 0x01},
	{0x487d, 0x00},
	{0x487e, 0x01},
	{0x487f, 0x00},
	{0x4880, 0x01},
	{0x4881, 0x0f},
	{0x4882, 0x01},
	{0x4883, 0x08},
	{0x4884, 0x01},
	{0x4885, 0x33},
	{0x4886, 0x01},
	{0x4887, 0x2c},
	{0x4888, 0x01},
	{0x4889, 0x67},
	{0x488a, 0x01},
	{0x488b, 0x52},
	{0x488c, 0x01},
	{0x488d, 0x8f},
	{0x488e, 0x01},
	{0x488f, 0x71},
	{0x4890, 0x01},
	{0x4891, 0xb1},
	{0x4892, 0x01},
	{0x4893, 0x89},
	{0x4894, 0x01},
	{0x4895, 0x8f},
	{0x4896, 0x01},
	{0x4897, 0x6f},
	{0x4898, 0x01},
	{0x4899, 0x58},
	{0x489a, 0x01},
	{0x489b, 0x41},
	{0x489c, 0x01},
	{0x489d, 0x26},
	{0x489e, 0x01},
	{0x489f, 0x1e},
	{0x48a0, 0x01},
	{0x48a1, 0x14},
	{0x48a2, 0x01},
	{0x48a3, 0x0e},
	{0x48a4, 0x01},
	{0x48a5, 0x20},
	{0x48a6, 0x01},
	{0x48a7, 0x18},
	{0x48a8, 0x01},
	{0x48a9, 0x46},
	{0x48aa, 0x01},
	{0x48ab, 0x3b},
	{0x48ac, 0x01},
	{0x48ad, 0x70},
	{0x48ae, 0x01},
	{0x48af, 0x5e},
	{0x48b0, 0x01},
	{0x48b1, 0x9b},
	{0x48b2, 0x01},
	{0x48b3, 0x77},
	{0x48b4, 0x01},
	{0x48b5, 0xc5},
	{0x48b6, 0x01},
	{0x48b7, 0x97},
	{0x48b8, 0x01},
	{0x48b9, 0xa5},
	{0x48ba, 0x01},
	{0x48bb, 0x80},
	{0x48bc, 0x01},
	{0x48bd, 0x75},
	{0x48be, 0x01},
	{0x48bf, 0x60},
	{0x48c0, 0x01},
	{0x48c1, 0x53},
	{0x48c2, 0x01},
	{0x48c3, 0x40},
	{0x48c4, 0x01},
	{0x48c5, 0x42},
	{0x48c6, 0x01},
	{0x48c7, 0x37},
	{0x48c8, 0x01},
	{0x48c9, 0x4b},
	{0x48ca, 0x01},
	{0x48cb, 0x3d},
	{0x48cc, 0x01},
	{0x48cd, 0x67},
	{0x48ce, 0x01},
	{0x48cf, 0x55},
	{0x48d0, 0x01},
	{0x48d1, 0x88},
	{0x48d2, 0x01},
	{0x48d3, 0x71},
	{0x48d4, 0x01},
	{0x48d5, 0x9f},
	{0x48d6, 0x01},
	{0x48d7, 0x7d},
	{0x48d8, 0x01},
	{0x48d9, 0xdf},
	{0x48da, 0x01},
	{0x48db, 0xaa},
	{0x48dc, 0x01},
	{0x48dd, 0xc4},
	{0x48de, 0x01},
	{0x48df, 0x99},
	{0x48e0, 0x01},
	{0x48e1, 0xa9},
	{0x48e2, 0x01},
	{0x48e3, 0x85},
	{0x48e4, 0x01},
	{0x48e5, 0x8b},
	{0x48e6, 0x01},
	{0x48e7, 0x74},
	{0x48e8, 0x01},
	{0x48e9, 0x7e},
	{0x48ea, 0x01},
	{0x48eb, 0x6a},
	{0x48ec, 0x01},
	{0x48ed, 0x82},
	{0x48ee, 0x01},
	{0x48ef, 0x70},
	{0x48f0, 0x01},
	{0x48f1, 0x98},
	{0x48f2, 0x01},
	{0x48f3, 0x7f},
	{0x48f4, 0x01},
	{0x48f5, 0xb0},
	{0x48f6, 0x01},
	{0x48f7, 0x8b},
	{0x48f8, 0x01},
	{0x48f9, 0xac},
	{0x48fa, 0x01},
	{0x48fb, 0x8c},
	{0x48fc, 0x01},
	{0x48fd, 0x8f},
	{0x48fe, 0x01},
	{0x48ff, 0x78},
	{0x4900, 0x01},
	{0x4901, 0x83},
	{0x4902, 0x01},
	{0x4903, 0x6b},
	{0x4904, 0x01},
	{0x4905, 0x6f},
	{0x4906, 0x01},
	{0x4907, 0x59},
	{0x4908, 0x01},
	{0x4909, 0x5b},
	{0x490a, 0x01},
	{0x490b, 0x4b},
	{0x490c, 0x01},
	{0x490d, 0x50},
	{0x490e, 0x01},
	{0x490f, 0x44},
	{0x4910, 0x01},
	{0x4911, 0x58},
	{0x4912, 0x01},
	{0x4913, 0x47},
	{0x4914, 0x01},
	{0x4915, 0x64},
	{0x4916, 0x01},
	{0x4917, 0x54},
	{0x4918, 0x01},
	{0x4919, 0x75},
	{0x491a, 0x01},
	{0x491b, 0x60},
	{0x491c, 0x01},
	{0x491d, 0x7b},
	{0x491e, 0x01},
	{0x491f, 0x67},
	{0x4920, 0x01},
	{0x4921, 0x8f},
	{0x4922, 0x01},
	{0x4923, 0x70},
	{0x4924, 0x01},
	{0x4925, 0x74},
	{0x4926, 0x01},
	{0x4927, 0x5a},
	{0x4928, 0x01},
	{0x4929, 0x51},
	{0x492a, 0x01},
	{0x492b, 0x3f},
	{0x492c, 0x01},
	{0x492d, 0x36},
	{0x492e, 0x01},
	{0x492f, 0x2c},
	{0x4930, 0x01},
	{0x4931, 0x2a},
	{0x4932, 0x01},
	{0x4933, 0x20},
	{0x4934, 0x01},
	{0x4935, 0x31},
	{0x4936, 0x01},
	{0x4937, 0x29},
	{0x4938, 0x01},
	{0x4939, 0x4a},
	{0x493a, 0x01},
	{0x493b, 0x38},
	{0x493c, 0x01},
	{0x493d, 0x69},
	{0x493e, 0x01},
	{0x493f, 0x56},
	{0x4940, 0x01},
	{0x4941, 0x7b},
	{0x4942, 0x01},
	{0x4943, 0x66},
	{0x4944, 0x01},
	{0x4945, 0x88},
	{0x4946, 0x01},
	{0x4947, 0x66},
	{0x4948, 0x01},
	{0x4949, 0x6a},
	{0x494a, 0x01},
	{0x494b, 0x51},
	{0x494c, 0x01},
	{0x494d, 0x3d},
	{0x494e, 0x01},
	{0x494f, 0x2f},
	{0x4950, 0x01},
	{0x4951, 0x18},
	{0x4952, 0x01},
	{0x4953, 0x11},
	{0x4954, 0x01},
	{0x4955, 0x09},
	{0x4956, 0x01},
	{0x4957, 0x06},
	{0x4958, 0x01},
	{0x4959, 0x13},
	{0x495a, 0x01},
	{0x495b, 0x0e},
	{0x495c, 0x01},
	{0x495d, 0x36},
	{0x495e, 0x01},
	{0x495f, 0x2a},
	{0x4960, 0x01},
	{0x4961, 0x5a},
	{0x4962, 0x01},
	{0x4963, 0x48},
	{0x4964, 0x01},
	{0x4965, 0x78},
	{0x4966, 0x01},
	{0x4967, 0x5f},
	{0x4968, 0x01},
	{0x4969, 0x93},
	{0x496a, 0x01},
	{0x496b, 0x70},
	{0x496c, 0x01},
	{0x496d, 0x6b},
	{0x496e, 0x01},
	{0x496f, 0x51},
	{0x4970, 0x01},
	{0x4971, 0x39},
	{0x4972, 0x01},
	{0x4973, 0x2e},
	{0x4974, 0x01},
	{0x4975, 0x12},
	{0x4976, 0x01},
	{0x4977, 0x0c},
	{0x4978, 0x01},
	{0x4979, 0x00},
	{0x497a, 0x01},
	{0x497b, 0x00},
	{0x497c, 0x01},
	{0x497d, 0x0a},
	{0x497e, 0x01},
	{0x497f, 0x08},
	{0x4980, 0x01},
	{0x4981, 0x2e},
	{0x4982, 0x01},
	{0x4983, 0x22},
	{0x4984, 0x01},
	{0x4985, 0x59},
	{0x4986, 0x01},
	{0x4987, 0x45},
	{0x4988, 0x01},
	{0x4989, 0x79},
	{0x498a, 0x01},
	{0x498b, 0x66},
	{0x498c, 0x01},
	{0x498d, 0x9d},
	{0x498e, 0x01},
	{0x498f, 0x78},
	{0x4990, 0x01},
	{0x4991, 0x7a},
	{0x4992, 0x01},
	{0x4993, 0x5e},
	{0x4994, 0x01},
	{0x4995, 0x4b},
	{0x4996, 0x01},
	{0x4997, 0x37},
	{0x4998, 0x01},
	{0x4999, 0x23},
	{0x499a, 0x01},
	{0x499b, 0x1b},
	{0x499c, 0x01},
	{0x499d, 0x11},
	{0x499e, 0x01},
	{0x499f, 0x0d},
	{0x49a0, 0x01},
	{0x49a1, 0x1a},
	{0x49a2, 0x01},
	{0x49a3, 0x13},
	{0x49a4, 0x01},
	{0x49a5, 0x3d},
	{0x49a6, 0x01},
	{0x49a7, 0x2e},
	{0x49a8, 0x01},
	{0x49a9, 0x65},
	{0x49aa, 0x01},
	{0x49ab, 0x53},
	{0x49ac, 0x01},
	{0x49ad, 0x83},
	{0x49ae, 0x01},
	{0x49af, 0x6b},
	{0x49b0, 0x01},
	{0x49b1, 0x9f},
	{0x49b2, 0x01},
	{0x49b3, 0x7e},
	{0x49b4, 0x01},
	{0x49b5, 0x8c},
	{0x49b6, 0x01},
	{0x49b7, 0x6a},
	{0x49b8, 0x01},
	{0x49b9, 0x65},
	{0x49ba, 0x01},
	{0x49bb, 0x50},
	{0x49bc, 0x01},
	{0x49bd, 0x45},
	{0x49be, 0x01},
	{0x49bf, 0x36},
	{0x49c0, 0x01},
	{0x49c1, 0x37},
	{0x49c2, 0x01},
	{0x49c3, 0x2c},
	{0x49c4, 0x01},
	{0x49c5, 0x3e},
	{0x49c6, 0x01},
	{0x49c7, 0x30},
	{0x49c8, 0x01},
	{0x49c9, 0x59},
	{0x49ca, 0x01},
	{0x49cb, 0x45},
	{0x49cc, 0x01},
	{0x49cd, 0x79},
	{0x49ce, 0x01},
	{0x49cf, 0x62},
	{0x49d0, 0x01},
	{0x49d1, 0x8a},
	{0x49d2, 0x01},
	{0x49d3, 0x6e},
	{0x49d4, 0x01},
	{0x49d5, 0xb6},
	{0x49d6, 0x01},
	{0x49d7, 0x91},
	{0x49d8, 0x01},
	{0x49d9, 0xa1},
	{0x49da, 0x01},
	{0x49db, 0x86},
	{0x49dc, 0x01},
	{0x49dd, 0x8b},
	{0x49de, 0x01},
	{0x49df, 0x74},
	{0x49e0, 0x01},
	{0x49e1, 0x79},
	{0x49e2, 0x01},
	{0x49e3, 0x61},
	{0x49e4, 0x01},
	{0x49e5, 0x70},
	{0x49e6, 0x01},
	{0x49e7, 0x5c},
	{0x49e8, 0x01},
	{0x49e9, 0x70},
	{0x49ea, 0x01},
	{0x49eb, 0x5f},
	{0x49ec, 0x01},
	{0x49ed, 0x82},
	{0x49ee, 0x01},
	{0x49ef, 0x71},
	{0x49f0, 0x01},
	{0x49f1, 0x96},
	{0x49f2, 0x01},
	{0x49f3, 0x7e},
	{0x49f4, 0x01},
	{0x49f5, 0x90},
	{0x49f6, 0x01},
	{0x49f7, 0x7b},
	{0x3A63, 0x01},
};

static struct msm_camera_i2c_reg_conf imx135_snap_settings[] = {
	{0x011E, 0x18},
	{0x011F, 0x00},
	{0x0301, 0x05},
	{0x0303, 0x01},
	{0x0305, 0x03},
	{0x0309, 0x05},
	{0x030B, 0x01},
	{0x030C, 0x00},
	{0x030D, 0x64},
	{0x030E, 0x01},
	{0x3A06, 0x11},
	{0x0108, 0x03},
	{0x0112, 0x0A},
	{0x0113, 0x0A},
	{0x0381, 0x01},
	{0x0383, 0x01},
	{0x0385, 0x01},
	{0x0387, 0x01},
	{0x0390, 0x00},
	{0x0391, 0x11},
	{0x0392, 0x00},
	{0x0401, 0x00},
	{0x0404, 0x00},
	{0x0405, 0x10},
	{0x4082, 0x01},
	{0x4083, 0x01},
	{0x7006, 0x04},
	{0x0700, 0x00},
	{0x3A63, 0x00},
	{0x4100, 0xE8},
	{0x4203, 0xFF},
	{0x4344, 0x00},
	{0x441C, 0x01},
	{0x0340, 0x0C},
	{0x0341, 0x46},
	{0x0342, 0x11},
	{0x0343, 0xDC},
	{0x0344, 0x00},
	{0x0345, 0x00},
	{0x0346, 0x00},
	{0x0347, 0x00},
	{0x0348, 0x10},
	{0x0349, 0x6F},
	{0x034A, 0x0C},
	{0x034B, 0x2F},
	{0x034C, 0x10},
	{0x034D, 0x70},
	{0x034E, 0x0C},
	{0x034F, 0x30},
	{0x0350, 0x00},
	{0x0351, 0x00},
	{0x0352, 0x00},
	{0x0353, 0x00},
	{0x0354, 0x10},
	{0x0355, 0x70},
	{0x0356, 0x0C},
	{0x0357, 0x30},
	{0x301D, 0x30},
	{0x3310, 0x10},
	{0x3311, 0x70},
	{0x3312, 0x0C},
	{0x3313, 0x30},
	{0x331C, 0x01},
	{0x331D, 0x68},
	{0x4084, 0x00},
	{0x4085, 0x00},
	{0x4086, 0x00},
	{0x4087, 0x00},
	{0x4400, 0x00},
	{0x0830, 0x7F},
	{0x0831, 0x37},
	{0x0832, 0x5F},
	{0x0833, 0x37},
	{0x0834, 0x37},
	{0x0835, 0x3F},
	{0x0836, 0xC7},
	{0x0837, 0x3F},
	{0x0839, 0x1F},
	{0x083A, 0x17},
	{0x083B, 0x02},
	{0x0202, 0x0C},
	{0x0203, 0x42},
	{0x0205, 0x00},
	{0x020E, 0x01},
	{0x020F, 0x00},
	{0x0210, 0x01},
	{0x0211, 0x00},
	{0x0212, 0x01},
	{0x0213, 0x00},
	{0x0214, 0x01},
	{0x0215, 0x00},
	{0x0230, 0x00},
	{0x0231, 0x00},
	{0x0233, 0x00},
	{0x0234, 0x00},
	{0x0235, 0x40},
	{0x0238, 0x00},
	{0x0239, 0x04},
	{0x023B, 0x00},
	{0x023C, 0x01},
	{0x33B0, 0x04},
	{0x33B1, 0x00},
	{0x33B3, 0x00},
	{0x33B4, 0x00},
	{0x3800, 0x00},
};

static struct msm_camera_i2c_reg_conf imx135_hdr_settings[] = {
	{0x011E, 0x18},
	{0x011F, 0x00},
	{0x0301, 0x05},
	{0x0303, 0x01},
	{0x0305, 0x03},
	{0x0309, 0x05},
	{0x030B, 0x02},
	{0x030C, 0x00},
	{0x030D, 0x64},
	{0x030E, 0x01},
	{0x3A06, 0x12},
	{0x0108, 0x03},
	{0x0112, 0x0E},
	{0x0113, 0x0A},
	{0x0381, 0x01},
	{0x0383, 0x01},
	{0x0385, 0x01},
	{0x0387, 0x01},
	{0x0390, 0x00},
	{0x0391, 0x11},
	{0x0392, 0x00},
	{0x0401, 0x00},
	{0x0404, 0x00},
	{0x0405, 0x10},
	{0x4082, 0x01},
	{0x4083, 0x01},
	{0x7006, 0x04},
	{0x0700, 0x01},
	{0x3A63, 0x00},
	{0x4100, 0xF8},
	{0x4203, 0xFF},
	{0x4344, 0x00},
	{0x441C, 0x01},
	{0x0340, 0x09},
	{0x0341, 0x1C},
	{0x0342, 0x11},
	{0x0343, 0xDC},
	{0x0344, 0x00},
	{0x0345, 0xB8},
	{0x0346, 0x01},
	{0x0347, 0xD8},
	{0x0348, 0x0F},
	{0x0349, 0xB7},
	{0x034A, 0x0A},
	{0x034B, 0x57},
	{0x034C, 0x07},
	{0x034D, 0x80},
	{0x034E, 0x04},
	{0x034F, 0x40},
	{0x0350, 0x00},
	{0x0351, 0x00},
	{0x0352, 0x00},
	{0x0353, 0x00},
	{0x0354, 0x07},
	{0x0355, 0x80},
	{0x0356, 0x04},
	{0x0357, 0x40},
	{0x301D, 0x30},
	{0x3310, 0x07},
	{0x3311, 0x80},
	{0x3312, 0x04},
	{0x3313, 0x40},
	{0x331C, 0x00},
	{0x331D, 0x80},
	{0x4084, 0x00},
	{0x4085, 0x00},
	{0x4086, 0x00},
	{0x4087, 0x00},
	{0x4400, 0x00},
	{0x0830, 0x67},
	{0x0831, 0x1F},
	{0x0832, 0x47},
	{0x0833, 0x1F},
	{0x0834, 0x1F},
	{0x0835, 0x17},
	{0x0836, 0x77},
	{0x0837, 0x27},
	{0x0839, 0x1F},
	{0x083A, 0x17},
	{0x083B, 0x02},
	{0x0202, 0x0C},
	{0x0203, 0x44},
	{0x0205, 0xE0},
	{0x020E, 0x01},
	{0x020F, 0x00},
	{0x0210, 0x01},
	{0x0211, 0x00},
	{0x0212, 0x01},
	{0x0213, 0x00},
	{0x0214, 0x01},
	{0x0215, 0x00},
	{0x0230, 0x01},
	{0x0231, 0x88},
	{0x0233, 0xE0},
	{0x0234, 0x00},
	{0x0235, 0x40},
	{0x0238, 0x01},
	{0x0239, 0x04},
	{0x023B, 0x03},
	{0x023C, 0x01},
	{0x33B0, 0x08},
	{0x33B1, 0x30},
	{0x33B3, 0x01},
	{0x33B4, 0x00},
	{0x3800, 0x00},
};

static struct v4l2_subdev_info imx135_subdev_info[] = {
	{
	.code		= V4L2_MBUS_FMT_SBGGR10_1X10,
	.colorspace	= V4L2_COLORSPACE_JPEG,
	.fmt		= 1,
	.order		= 0,
	},
};

static struct msm_camera_i2c_conf_array imx135_init_conf[] = {
	{&imx135_recommend_settings[0],
	ARRAY_SIZE(imx135_recommend_settings), 0, MSM_CAMERA_I2C_BYTE_DATA},
	{&imx135_LSCTable_settings[0],
	ARRAY_SIZE(imx135_LSCTable_settings), 0, MSM_CAMERA_I2C_BYTE_DATA}
};

static struct msm_camera_i2c_conf_array imx135_confs[] = {
	{&imx135_snap_settings[0],
	ARRAY_SIZE(imx135_snap_settings), 0, MSM_CAMERA_I2C_BYTE_DATA},
	{&imx135_snap_settings[0],
	ARRAY_SIZE(imx135_snap_settings), 0, MSM_CAMERA_I2C_BYTE_DATA},
	{&imx135_hdr_settings[0],
	ARRAY_SIZE(imx135_hdr_settings), 0, MSM_CAMERA_I2C_BYTE_DATA},
	{&imx135_60fps_settings[0],
	ARRAY_SIZE(imx135_60fps_settings), 0, MSM_CAMERA_I2C_BYTE_DATA},
	{&imx135_90fps_settings[0],
	ARRAY_SIZE(imx135_90fps_settings), 0, MSM_CAMERA_I2C_BYTE_DATA},
};

static struct msm_sensor_output_info_t imx135_dimensions[] = {
	{
		.x_output = 4208,
		.y_output = 3120,
		.line_length_pclk = 4572,
		.frame_length_lines = 3142,
		.vt_pixel_clk = 319000000,
		.op_pixel_clk = 319680000,
		.binning_factor = 1,
	},
	{
		.x_output = 4208,
		.y_output = 3120,
		.line_length_pclk = 4572,
		.frame_length_lines = 3142,
		.vt_pixel_clk = 319000000,
		.op_pixel_clk = 319680000,
		.binning_factor = 1,
	},
	{
		.x_output = 1920,
		.y_output = 1088,
		.line_length_pclk = 4572,
		.frame_length_lines = 2332,
		.vt_pixel_clk = 319600000,
		.op_pixel_clk = 319600000,
		.binning_factor = 1,
	},
	{
		.x_output = 1312,
		.y_output = 736,
		.line_length_pclk = 4572,
		.frame_length_lines = 1178,
		.vt_pixel_clk = 360000000,
		.op_pixel_clk = 319600000,
		.binning_factor = 1,
	},
	{
		.x_output = 1312,
		.y_output = 736,
		.line_length_pclk = 4572,
		.frame_length_lines = 776,
		.vt_pixel_clk = 319000000,
		.op_pixel_clk = 319600000,
		.binning_factor = 1,
	},
};

static struct msm_camera_csid_vc_cfg imx135_cid_cfg[] = {
	{0, CSI_RAW10, CSI_DECODE_10BIT},
	{1, 0x35,      CSI_DECODE_10BIT},
	{2, CSI_EMBED_DATA, CSI_DECODE_8BIT},
};

static struct msm_camera_csi2_params imx135_csi_params = {
	.csid_params = {
		.lane_assign = 0xe4,
		.lane_cnt = 4,
		.lut_params = {
			.num_cid = ARRAY_SIZE(imx135_cid_cfg),
			.vc_cfg = imx135_cid_cfg,
		},
	},
	.csiphy_params = {
		.lane_cnt = 4,
		.settle_cnt = 0x12,
	},
};

static struct msm_camera_csi2_params *imx135_csi_params_array[] = {
	&imx135_csi_params,
	&imx135_csi_params,
	&imx135_csi_params,
	&imx135_csi_params,
	&imx135_csi_params,
	&imx135_csi_params,
};

static struct msm_sensor_output_reg_addr_t imx135_reg_addr = {
	.x_output = 0x34C,
	.y_output = 0x34E,
	.line_length_pclk = 0x342,
	.frame_length_lines = 0x340,
};

static struct msm_sensor_id_info_t imx135_id_info = {
	.sensor_id_reg_addr = 0x0016,
	.sensor_id = 0x0135,
};

static struct msm_sensor_exp_gain_info_t imx135_exp_gain_info = {
	.coarse_int_time_addr = 0x202,
	.global_gain_addr = 0x205,
	.vert_offset = 4,
};

static const struct i2c_device_id imx135_i2c_id[] = {
	{SENSOR_NAME, (kernel_ulong_t)&imx135_s_ctrl},
	{ }
};

static struct i2c_driver imx135_i2c_driver = {
	.id_table = imx135_i2c_id,
	.probe  = msm_sensor_i2c_probe,
	.driver = {
		.name = SENSOR_NAME,
	},
};

static struct msm_camera_i2c_client imx135_sensor_i2c_client = {
	.addr_type = MSM_CAMERA_I2C_WORD_ADDR,
};

static int __init msm_sensor_init_module(void)
{
	return i2c_add_driver(&imx135_i2c_driver);
}

static struct v4l2_subdev_core_ops imx135_subdev_core_ops = {
	.ioctl = msm_sensor_subdev_ioctl,
	.s_power = msm_sensor_power,
};

static struct v4l2_subdev_video_ops imx135_subdev_video_ops = {
	.enum_mbus_fmt = msm_sensor_v4l2_enum_fmt,
};

static struct v4l2_subdev_ops imx135_subdev_ops = {
	.core = &imx135_subdev_core_ops,
	.video  = &imx135_subdev_video_ops,
};

static uint16_t otp_readed;
static uint16_t r_over_g, b_over_g, b_over_r;
static int32_t imx135_match_id(struct msm_sensor_ctrl_t *s_ctrl)
{
	int32_t  rc = 0;
	uint16_t chipid = 0;
	uint16_t byte = 0;
	int32_t i = 0;

	rc = msm_camera_i2c_read(
		s_ctrl->sensor_i2c_client,
		s_ctrl->sensor_id_info->sensor_id_reg_addr, &chipid,
		MSM_CAMERA_I2C_WORD_DATA);
	if (rc < 0) {
		pr_err("%s: %s: read id failed\n", __func__,
		s_ctrl->sensordata->sensor_name);
		return rc;
	}

	if (chipid == 0x0135) {
		s_ctrl->sensordata->actuator_info->cam_name = 2;
		if (otp_readed == 1)
			return 0;
	} else {
		pr_err("imx_sensor_match_id chip id doesnot match 0x%04x\n", chipid);
		return -ENODEV;
	}

	for (i = 2; i >= 0; i--) {
		msm_camera_i2c_write(s_ctrl->sensor_i2c_client, 0x3B02, 10+i , MSM_CAMERA_I2C_BYTE_DATA);
		msm_camera_i2c_write(s_ctrl->sensor_i2c_client, 0x3B00, 0x01, MSM_CAMERA_I2C_BYTE_DATA);
		udelay(10);
		for (rc = 3; rc > 0; rc--) {
			msm_camera_i2c_read(s_ctrl->sensor_i2c_client, 0x3B01, &byte, MSM_CAMERA_I2C_BYTE_DATA);
			if (byte == 1)
				break;
			udelay(10);
		}

		msm_camera_i2c_read(s_ctrl->sensor_i2c_client, 0x3B04, &chipid, MSM_CAMERA_I2C_WORD_DATA);
		msm_camera_i2c_read(s_ctrl->sensor_i2c_client, 0x3B06, &r_over_g, MSM_CAMERA_I2C_WORD_DATA);
		msm_camera_i2c_read(s_ctrl->sensor_i2c_client, 0x3B08, &b_over_g, MSM_CAMERA_I2C_WORD_DATA);
		msm_camera_i2c_read(s_ctrl->sensor_i2c_client, 0x3B0A, &b_over_r, MSM_CAMERA_I2C_WORD_DATA);

		if ((chipid == 0x0D01) || (chipid == 0x1D01))
			break;
	}
	s_ctrl->msm_sensor_reg->sensor_wb_calib.r_over_g = r_over_g;
	s_ctrl->msm_sensor_reg->sensor_wb_calib.b_over_g = b_over_g;
	s_ctrl->msm_sensor_reg->sensor_wb_calib.gr_over_gb = b_over_r;
	otp_readed = 1;

	return 0;
}

static int32_t imx135_write_exp_gain(struct msm_sensor_ctrl_t *s_ctrl,
		uint16_t gain, uint32_t line)
{
	uint32_t fl_lines;
	uint8_t  offset;
	uint16_t digital_gain_int = 0;
	uint16_t digital_gain = 0x100;

	fl_lines = s_ctrl->curr_frame_length_lines;
	fl_lines = (fl_lines * s_ctrl->fps_divider) / Q10;
	offset = s_ctrl->sensor_exp_gain_info->vert_offset;

	if (line > (fl_lines - offset))
		fl_lines = line + offset;

	s_ctrl->func_tbl->sensor_group_hold_on(s_ctrl);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
		s_ctrl->sensor_output_reg_addr->frame_length_lines, fl_lines,
		MSM_CAMERA_I2C_WORD_DATA);

	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
		s_ctrl->sensor_exp_gain_info->coarse_int_time_addr, line,
		MSM_CAMERA_I2C_WORD_DATA);

	if (gain > 224) {
		digital_gain_int = (gain & 0x00FF) - 224;
		digital_gain = (digital_gain_int << 8) + ((gain & 0xFF00) >> 8);
		gain = 224;
	}
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
				s_ctrl->sensor_exp_gain_info->global_gain_addr, gain,
				MSM_CAMERA_I2C_BYTE_DATA);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client, 0x020E, digital_gain, MSM_CAMERA_I2C_WORD_DATA);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client, 0x0210, digital_gain, MSM_CAMERA_I2C_WORD_DATA);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client, 0x0212, digital_gain, MSM_CAMERA_I2C_WORD_DATA);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client, 0x0214, digital_gain, MSM_CAMERA_I2C_WORD_DATA);

	s_ctrl->func_tbl->sensor_group_hold_off(s_ctrl);
	return 0;
}

int32_t imx135_write_exp_gain_hdr(struct msm_sensor_ctrl_t *s_ctrl,
		uint16_t gain, uint32_t line, int32_t luma_avg, uint16_t fgain)
{
	uint32_t fl_lines;
	uint8_t offset;
	uint16_t luma_delta;
	uint32_t atr_threshold = 13;
	uint16_t shortshutter_gain = 0;
	uint32_t shortshutter = 0;
	uint32_t remainder = 0;
	uint16_t shortshutter_expratio = 8;

	uint32_t atr_offset = 0;
	uint16_t atr_out_noise = 0xA0;
	uint16_t atr_out_mid = 0x800;
	uint16_t atr_noise_brate = 0;
	uint16_t atr_mid_brate = 0;
	uint16_t luma_delta_h;
	uint16_t luma_delta_l;
	uint32_t r, ev;

	luma_delta = fgain;
	fgain = Q8*256/(256-gain);

	if (fgain < Q8 * 2) {
		ev = line * fgain;
		r  = line / shortshutter_expratio;
		line = r * shortshutter_expratio;
		fgain  = ev / line;
		gain = 256 - (256 * Q8 / fgain);

		shortshutter = line / 8;
		shortshutter_gain = gain;
	} else {
		shortshutter = (line * fgain) / (Q8 * shortshutter_expratio);
		remainder = (line * fgain / shortshutter_expratio) - (shortshutter * Q8);
		if (remainder >= 128)
			shortshutter++;
	}

	fl_lines = s_ctrl->curr_frame_length_lines;
	fl_lines = (fl_lines * s_ctrl->fps_divider) / Q10;
	offset = s_ctrl->sensor_exp_gain_info->vert_offset;
	if (line > (fl_lines - offset))
		fl_lines = line + offset;

	if (s_ctrl->curr_res == MSM_SENSOR_RES_2)
		if (line < 24)
			line = 24;

	s_ctrl->func_tbl->sensor_group_hold_on(s_ctrl);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
		s_ctrl->sensor_output_reg_addr->frame_length_lines, fl_lines,
		MSM_CAMERA_I2C_WORD_DATA);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
		s_ctrl->sensor_exp_gain_info->coarse_int_time_addr, line,
		MSM_CAMERA_I2C_WORD_DATA);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
		s_ctrl->sensor_exp_gain_info->global_gain_addr, gain,
	MSM_CAMERA_I2C_BYTE_DATA);

	if (s_ctrl->curr_res == MSM_SENSOR_RES_2) {
		msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
				SHORT_GAIN_BYTE_ADDR, shortshutter_gain,
				MSM_CAMERA_I2C_BYTE_DATA);

		msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
				SHORT_SHUTTER_WORD_ADDR, shortshutter, MSM_CAMERA_I2C_WORD_DATA);
		luma_delta_h = (luma_delta >> 8) & 0xff;
		luma_delta_l = luma_delta & 0xff;

		if (luma_avg < atr_threshold) {
			atr_out_noise = ((0xA0 * (luma_avg)) / (atr_threshold)) / 2 + 0x50 + atr_offset;
			atr_out_mid = ((0x800 * (luma_avg)) / (atr_threshold)) / 2 + 0x400 + atr_offset;
		} else {
			atr_out_noise = 0xA0 + atr_offset;
			atr_out_mid = 0x800 + atr_offset;
		}
		atr_noise_brate = 0x18 * 1 + atr_offset;
		atr_mid_brate = 0x18 * 1 + atr_offset;

		msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
				TC_OUT_NOISE_REGADDR, atr_out_noise, MSM_CAMERA_I2C_WORD_DATA);
		msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
				TC_OUT_MID_REGADDR, atr_out_mid, MSM_CAMERA_I2C_WORD_DATA);
		msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
				TC_NOISE_BRATE_REGADDR, atr_noise_brate, MSM_CAMERA_I2C_BYTE_DATA);
		msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
				TC_MID_BRATE_REGADDR, atr_mid_brate, MSM_CAMERA_I2C_BYTE_DATA);
		msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
				TC_SWITCH_REGADDR, 0x00, MSM_CAMERA_I2C_BYTE_DATA);

		luma_delta_h *= 4;
		luma_delta_l *= 4;
		luma_avg *= 4;
		if (((luma_delta_h - luma_avg) < (200 + luma_delta_hys)) && (luma_delta_h < 700)) {
			luma_delta_hys = 30;
			if (luma_delta_h < 959)
				luma_delta_h = 959;
			if (luma_delta_l > 60)
				luma_delta_l = 60;
			if (luma_delta_l < 8)
				luma_delta_l = 8;
			if (luma_avg <= luma_delta_l)
				luma_avg = luma_delta_l + 1;
			msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
					DBG_AVE_H_REGADDR, luma_avg, MSM_CAMERA_I2C_WORD_DATA);
			msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
					DBG_MAX_H_REGADDR, luma_delta_h, MSM_CAMERA_I2C_WORD_DATA);
			msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
					DBG_MIN_H_REGADDR, luma_delta_l, MSM_CAMERA_I2C_WORD_DATA);
			msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
					DBG_SEL, 1, MSM_CAMERA_I2C_BYTE_DATA);
		} else {
			luma_delta_hys = 0;
			msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
					DBG_SEL, 0, MSM_CAMERA_I2C_BYTE_DATA);
		}
	}
	s_ctrl->func_tbl->sensor_group_hold_off(s_ctrl);
	return 0;
}

int32_t imx135_hdr_update(struct msm_sensor_ctrl_t *s_ctrl,
						  struct sensor_hdr_update_parm_t *update_parm)
{
	int i;

	switch (update_parm->type) {
	case SENSOR_HDR_UPDATE_AWB:
		msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
							 ABS_GAIN_R_WORD_ADDR, update_parm->awb_gain_r,
							 MSM_CAMERA_I2C_WORD_DATA);
		msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
							 ABS_GAIN_B_WORD_ADDR, update_parm->awb_gain_b,
							 MSM_CAMERA_I2C_WORD_DATA);
		break;
	case SENSOR_HDR_UPDATE_LSC:
		for (i = 0; i < LSC_TABLE_LEN_BYTES; i++) {
			msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
						 LSC_TABLE_START_ADDR + i, update_parm->lsc_table[i],
						 MSM_CAMERA_I2C_BYTE_DATA);
		}
		msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
							 EN_LSC_BYTE_ADDR, 0x1F,
							 MSM_CAMERA_I2C_BYTE_DATA);
		msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
							 LSC_ENABLE_BYTE_ADDR, 0x01,
							 MSM_CAMERA_I2C_BYTE_DATA);
		msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
							 RAM_SEL_TOGGLE_BYTE_ADDR, 0x01,
							 MSM_CAMERA_I2C_BYTE_DATA);

		break;
	default:
		pr_err("%s: invalid HDR update type %d\n", __func__, update_parm->type);
		return -EINVAL;
	}
	return 0;
}

static struct msm_sensor_fn_t imx135_func_tbl = {
	.sensor_start_stream = msm_sensor_start_stream,
	.sensor_stop_stream = msm_sensor_stop_stream,
	.sensor_group_hold_on = msm_sensor_group_hold_on,
	.sensor_group_hold_off = msm_sensor_group_hold_off,
	.sensor_set_fps = msm_sensor_set_fps,
	.sensor_write_exp_gain = imx135_write_exp_gain,
	.sensor_write_snapshot_exp_gain = imx135_write_exp_gain,
	.sensor_write_exp_gain_hdr = imx135_write_exp_gain_hdr,
	.sensor_write_snapshot_exp_gain_hdr = imx135_write_exp_gain_hdr,
	.sensor_setting = msm_sensor_setting,
	.sensor_set_sensor_mode = msm_sensor_set_sensor_mode,
	.sensor_mode_init = msm_sensor_mode_init,
	.sensor_get_output_info = msm_sensor_get_output_info,
	.sensor_config = msm_sensor_config,
	.sensor_power_up = msm_sensor_power_up,
	.sensor_power_down = msm_sensor_power_down,
	.sensor_get_csi_params = msm_sensor_get_csi_params,
	.sensor_hdr_update = imx135_hdr_update,
	.sensor_match_id   = imx135_match_id,
};

static struct msm_sensor_reg_t imx135_regs = {
	.default_data_type = MSM_CAMERA_I2C_BYTE_DATA,
	.start_stream_conf = imx135_start_settings,
	.start_stream_conf_size = ARRAY_SIZE(imx135_start_settings),
	.stop_stream_conf = imx135_stop_settings,
	.stop_stream_conf_size = ARRAY_SIZE(imx135_stop_settings),
	.group_hold_on_conf = imx135_groupon_settings,
	.group_hold_on_conf_size = ARRAY_SIZE(imx135_groupon_settings),
	.group_hold_off_conf = imx135_groupoff_settings,
	.group_hold_off_conf_size =
		ARRAY_SIZE(imx135_groupoff_settings),
	.init_settings = &imx135_init_conf[0],
	.init_size = ARRAY_SIZE(imx135_init_conf),
	.mode_settings = &imx135_confs[0],
	.output_settings = &imx135_dimensions[0],
	.num_conf = ARRAY_SIZE(imx135_confs),
};

static struct msm_sensor_ctrl_t imx135_s_ctrl = {
	.msm_sensor_reg = &imx135_regs,
	.sensor_i2c_client = &imx135_sensor_i2c_client,
	.sensor_i2c_addr = 0x20,
	.sensor_output_reg_addr = &imx135_reg_addr,
	.sensor_id_info = &imx135_id_info,
	.sensor_exp_gain_info = &imx135_exp_gain_info,
	.cam_mode = MSM_SENSOR_MODE_INVALID,
	.csi_params = &imx135_csi_params_array[0],
	.msm_sensor_mutex = &imx135_mut,
	.sensor_i2c_driver = &imx135_i2c_driver,
	.sensor_v4l2_subdev_info = imx135_subdev_info,
	.sensor_v4l2_subdev_info_size = ARRAY_SIZE(imx135_subdev_info),
	.sensor_v4l2_subdev_ops = &imx135_subdev_ops,
	.func_tbl = &imx135_func_tbl,
	.clk_rate = MSM_SENSOR_MCLK_24HZ,
};

module_init(msm_sensor_init_module);
MODULE_DESCRIPTION("Sony 13MP Bayer sensor driver");
MODULE_LICENSE("GPL v2");
