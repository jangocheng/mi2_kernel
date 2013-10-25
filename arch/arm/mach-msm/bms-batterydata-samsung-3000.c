/* Copyright (c) 2012-2013, Xiaomi corporation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#include <linux/mfd/pm8xxx/pm8921-bms.h>

static struct single_row_lut fcc_temp = {
	.x	= {-20, -10, 0, 5, 10, 15, 20, 25, 30, 40, 60},
	.y	= {3080, 3077, 3077, 3076, 3072, 3073, 3074, 3073, 3070, 3069, 3061},
	.cols	= 11
};

static struct single_row_lut fcc_sf = {
	.x	= {0},
	.y	= {100},
	.cols	= 1
};

static struct sf_lut rbatt_sf = {
	.rows	= 28,
	.cols	= 11,
	.row_entries	= {-20, -10, 0, 5, 10, 15, 20, 25, 30, 40, 60},
	.percent	= {100, 95, 90, 85, 80, 75, 70, 65, 60, 55, 50, 45, 40, 35, 30, 25, 20, 15, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1},
	.sf		= {
		{421, 344, 188, 151, 128, 114, 105, 100, 96, 94, 96},
		{731, 344, 197, 158, 133, 118, 108, 102, 99, 95, 97},
		{734, 343, 209, 168, 140, 123, 112, 105, 101, 97, 98},
		{709, 330, 210, 175, 147, 128, 115, 108, 103, 98, 100},
		{691, 320, 204, 180, 154, 134, 121, 111, 106, 101, 101},
		{690, 321, 196, 182, 162, 142, 127, 117, 110, 104, 103},
		{688, 320, 190, 167, 164, 150, 136, 124, 115, 107, 105},
		{685, 322, 186, 156, 143, 142, 140, 133, 126, 115, 110},
		{686, 322, 185, 152, 131, 118, 112, 108, 106, 103, 102},
		{691, 324, 185, 150, 129, 116, 107, 102, 99, 96, 99},
		{709, 322, 186, 151, 130, 116, 107, 102, 99, 97, 100},
		{740, 321, 187, 152, 131, 117, 109, 104, 101, 99, 102},
		{782, 324, 190, 154, 132, 119, 111, 106, 103, 102, 104},
		{834, 333, 198, 160, 136, 123, 114, 109, 104, 102, 106},
		{898, 349, 205, 169, 144, 126, 118, 112, 107, 99, 101},
		{974, 374, 210, 172, 148, 130, 118, 108, 103, 99, 100},
		{1071, 413, 195, 166, 143, 125, 113, 106, 102, 99, 101},
		{1201, 479, 204, 157, 132, 118, 108, 102, 99, 97, 101},
		{1169, 407, 201, 156, 131, 116, 108, 102, 100, 97, 101},
		{1065, 414, 205, 158, 132, 118, 109, 104, 101, 99, 102},
		{1113, 425, 211, 162, 136, 121, 112, 106, 103, 101, 104},
		{1172, 438, 220, 169, 141, 125, 115, 109, 105, 103, 107},
		{1247, 457, 231, 178, 149, 131, 119, 113, 109, 105, 108},
		{1339, 478, 242, 190, 159, 137, 124, 115, 111, 105, 105},
		{1460, 512, 244, 189, 153, 132, 119, 111, 106, 102, 105},
		{1712, 572, 248, 186, 152, 131, 119, 112, 108, 105, 107},
		{2440, 699, 272, 199, 162, 140, 126, 118, 114, 110, 112},
		{4219, 1132, 342, 242, 204, 184, 149, 143, 136, 164, 359},
	}
};

static struct pc_temp_ocv_lut pc_temp_ocv = {
	.rows		= 29,
	.cols		= 11,
	.temp		= {-20, -10, 0, 5, 10, 15, 20, 25, 30, 40, 60},
	.percent	= {100, 95, 90, 85, 80, 75, 70, 65, 60, 55, 50, 45, 40, 35, 30, 25, 20, 15, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0},
	.ocv		= {
				{4185, 4184, 4183, 4183, 4182, 4182, 4181, 4180, 4180, 4177, 4172},
				{4110, 4122, 4124, 4125, 4125, 4124, 4124, 4124, 4123, 4122, 4118},
				{4057, 4073, 4079, 4079, 4079, 4079, 4079, 4078, 4078, 4076, 4072},
				{4000, 4020, 4034, 4036, 4037, 4037, 4037, 4036, 4036, 4034, 4030},
				{3948, 3971, 3989, 3995, 3996, 3997, 3997, 3997, 3997, 3996, 3992},
				{3915, 3933, 3949, 3960, 3963, 3963, 3964, 3963, 3963, 3962, 3957},
				{3882, 3902, 3912, 3925, 3932, 3933, 3933, 3933, 3933, 3931, 3926},
				{3850, 3873, 3879, 3886, 3899, 3903, 3905, 3905, 3905, 3903, 3898},
				{3823, 3847, 3852, 3853, 3859, 3866, 3872, 3874, 3875, 3874, 3870},
				{3799, 3824, 3829, 3829, 3829, 3829, 3830, 3831, 3832, 3832, 3828},
				{3783, 3804, 3809, 3809, 3809, 3809, 3808, 3808, 3808, 3807, 3805},
				{3770, 3785, 3793, 3794, 3794, 3793, 3793, 3793, 3793, 3792, 3790},
				{3759, 3770, 3780, 3781, 3781, 3781, 3781, 3781, 3780, 3780, 3777},
				{3746, 3758, 3771, 3773, 3773, 3773, 3772, 3771, 3771, 3769, 3766},
				{3733, 3745, 3763, 3766, 3767, 3766, 3766, 3765, 3763, 3759, 3752},
				{3718, 3731, 3750, 3756, 3758, 3757, 3756, 3754, 3752, 3745, 3730},
				{3701, 3719, 3728, 3734, 3738, 3738, 3736, 3733, 3731, 3723, 3708},
				{3678, 3705, 3704, 3706, 3707, 3706, 3703, 3701, 3699, 3692, 3679},
				{3646, 3687, 3688, 3686, 3683, 3681, 3679, 3676, 3674, 3668, 3656},
				{3636, 3681, 3685, 3683, 3681, 3679, 3677, 3675, 3672, 3667, 3654},
				{3625, 3674, 3681, 3680, 3679, 3677, 3675, 3672, 3670, 3664, 3652},
				{3611, 3664, 3676, 3676, 3676, 3674, 3672, 3670, 3667, 3661, 3649},
				{3595, 3651, 3668, 3670, 3670, 3669, 3667, 3664, 3662, 3655, 3640},
				{3575, 3631, 3651, 3656, 3655, 3654, 3652, 3648, 3646, 3636, 3611},
				{3548, 3600, 3619, 3623, 3620, 3618, 3615, 3610, 3608, 3593, 3564},
				{3508, 3554, 3566, 3567, 3562, 3560, 3557, 3552, 3550, 3533, 3500},
				{3446, 3484, 3488, 3488, 3480, 3478, 3476, 3470, 3470, 3449, 3411},
				{3332, 3369, 3366, 3364, 3353, 3351, 3350, 3342, 3345, 3312, 3267},
				{3000, 3000, 3000, 3000, 3000, 3000, 3000, 3000, 3000, 3000, 3000}
	}
};

struct pm8921_bms_battery_data  samsung_3000_data = {
	.fcc			= 3210,
	.fcc_temp_lut		= &fcc_temp,
	.fcc_sf_lut		= &fcc_sf,
	.pc_temp_ocv_lut	= &pc_temp_ocv,
	.rbatt_sf_lut		= &rbatt_sf,
	.default_rbatt_mohm	= 130,
};
