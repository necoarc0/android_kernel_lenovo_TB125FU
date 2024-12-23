#ifndef __HARDWARE_H__
#define __HARDWARE_H__

#define HARDWARE_MAX_ITEM_LONGTH		64

enum{
	HARDWARE_LCD = 0,
	HARDWARE_TP,
        HARDWARE_HALL,
	HARDWARE_FLASH,
	HARDWARE_FRONT_CAM,//front
	HARDWARE_BACK_CAM,//back
	HARDWARE_BACK_SUBCAM,//depth
	HARDWARE_WIDE_ANGLE_CAM,//wide
	HARDWARE_MACRO_CAM,//macro
	HARDWARE_BT,
	HARDWARE_WIFI,
	HARDWARE_ACCELEROMETER,
	HARDWARE_ALSPS,
	HARDWARE_GYROSCOPE,
	HARDWARE_MAGNETOMETER,
	HARDWARE_SAR,//bug 417945 , add sar info, chenrongli.wt, 20181218
	HARDWARE_GPS,
	HARDWARE_FM,
        HARDWARE_NFC,//bug 493726, add NFC info, dulinyu.wt, 20191018
	HARDWARE_BATTERY_ID,
	HARDWARE_CHARGER_IC_INFO,  //bug 436809  modify getian.wt 20190403 Add charger IC model information in factory mode
	HARDWARE_CHARGER_IC_SLAVE_INFO,
	HARDWARE_BACK_CAM_MOUDULE_ID,//back
	HARDWARE_BACK_SUBCAM_MODULEID,//depth
	HARDWARE_FRONT_CAM_MOUDULE_ID,//front
	HARDWARE_WIDE_ANGLE_CAM_MOUDULE_ID,//wide
	HARDWARE_MACRO_CAM_MOUDULE_ID,//macro
	HARDWARE_BOARD_ID,
	HARDWARE_HARDWARE_ID,
	HARDWARE_SMART_PA_ID,
	HARDWARE_BMS_GAUGE_ID, //Bug493560,lili5.wt,ADD,20191112,Add fuel gauge information in factory mode
	HARDWARE_MAX_ITEM
};


#define HARDWARE_ID                               'H'
#define HARDWARE_LCD_GET                          _IOWR(HARDWARE_ID, 0x01, char[HARDWARE_MAX_ITEM_LONGTH])      //  LCD
#define HARDWARE_TP_GET                           _IOWR(HARDWARE_ID, 0x02, char[HARDWARE_MAX_ITEM_LONGTH])      //  TP
#define HARDWARE_FLASH_GET                        _IOWR(HARDWARE_ID, 0x03, char[HARDWARE_MAX_ITEM_LONGTH])      //  FLASH
#define HARDWARE_FRONT_CAM_GET                    _IOWR(HARDWARE_ID, 0x04, char[HARDWARE_MAX_ITEM_LONGTH])      //front
#define HARDWARE_BACK_CAM_GET                     _IOWR(HARDWARE_ID, 0x05, char[HARDWARE_MAX_ITEM_LONGTH])      //back
#define HARDWARE_ACCELEROMETER_GET                _IOWR(HARDWARE_ID, 0x06, char[HARDWARE_MAX_ITEM_LONGTH])      //  加速度传感器
#define HARDWARE_ALSPS_GET                        _IOWR(HARDWARE_ID, 0x07, char[HARDWARE_MAX_ITEM_LONGTH])      //  距离光感
#define HARDWARE_GYROSCOPE_GET                    _IOWR(HARDWARE_ID, 0x08, char[HARDWARE_MAX_ITEM_LONGTH])      //  陀螺仪
#define HARDWARE_MAGNETOMETER_GET                 _IOWR(HARDWARE_ID, 0x09, char[HARDWARE_MAX_ITEM_LONGTH])      //  地磁
#define HARDWARE_BT_GET                           _IOWR(HARDWARE_ID, 0x10, char[HARDWARE_MAX_ITEM_LONGTH])      //  蓝牙
#define HARDWARE_WIFI_GET                         _IOWR(HARDWARE_ID, 0x11, char[HARDWARE_MAX_ITEM_LONGTH])      //  WIFI
#define HARDWARE_GPS_GET                          _IOWR(HARDWARE_ID, 0x12, char[HARDWARE_MAX_ITEM_LONGTH])      //  GPS
#define HARDWARE_FM_GET                           _IOWR(HARDWARE_ID, 0x13, char[HARDWARE_MAX_ITEM_LONGTH])      //  FM
#define HARDWARE_BATTERY_ID_GET                   _IOWR(HARDWARE_ID, 0x15, char[HARDWARE_MAX_ITEM_LONGTH])      //  电池
#define HARDWARE_BACK_CAM_MOUDULE_ID_GET          _IOWR(HARDWARE_ID, 0x16, char[HARDWARE_MAX_ITEM_LONGTH])      //back
#define HARDWARE_FRONT_CAM_MODULE_ID_GET          _IOWR(HARDWARE_ID, 0x17, char[HARDWARE_MAX_ITEM_LONGTH])      //front
#define HARDWARE_BOARD_ID_GET                     _IOWR(HARDWARE_ID, 0x18, char[HARDWARE_MAX_ITEM_LONGTH])      //  board id
#define HARDWARE_FRONT_FISH_CAM_GET               _IOWR(HARDWARE_ID, 0x19, char[HARDWARE_MAX_ITEM_LONGTH])      // 前鱼眼
#define HARDWARE_BACK_FISH_CAM_GET                _IOWR(HARDWARE_ID, 0x1A, char[HARDWARE_MAX_ITEM_LONGTH])      //  后鱼眼
#define HARDWARE_HALL_GET                         _IOWR(HARDWARE_ID, 0x1B, char[HARDWARE_MAX_ITEM_LONGTH])      //  hall sensor
#define HARDWARE_PRESSURE_GET                     _IOWR(HARDWARE_ID, 0x1C, char[HARDWARE_MAX_ITEM_LONGTH])      //  气压传感器
#define HARDWARE_NFC_GET                          _IOWR(HARDWARE_ID, 0x1D, char[HARDWARE_MAX_ITEM_LONGTH])      //  NFC
#define HARDWARE_FRONT_SUBCAM_GET                 _IOWR(HARDWARE_ID, 0x1E, char[HARDWARE_MAX_ITEM_LONGTH])      //  前摄是双摄，前副摄像头
#define HARDWARE_BACK_SUBCAM_GET                  _IOWR(HARDWARE_ID, 0x1F, char[HARDWARE_MAX_ITEM_LONGTH])     //depth
#define HARDWARE_FRONT_FISH_CAM_MOUDULE_ID_GET    _IOWR(HARDWARE_ID, 0x20, char[HARDWARE_MAX_ITEM_LONGTH])      //  前鱼眼模组厂
#define HARDWARE_BACK_FISH_CAM_MOUDULE_ID_GET     _IOWR(HARDWARE_ID, 0x21, char[HARDWARE_MAX_ITEM_LONGTH])      // 后鱼眼模组厂
#define HARDWARE_HARDWARE_ID_GET                  _IOWR(HARDWARE_ID, 0x22, char[HARDWARE_MAX_ITEM_LONGTH])      //hardwareid

#define HARDWARE_FRONT_SUBCAM_MODULEID_GET        _IOWR(HARDWARE_ID, 0x23, char[HARDWARE_MAX_ITEM_LONGTH])      //  前摄是双摄，前副摄像头
#define HARDWARE_BACK_SUBCAM_MODULEID_GET         _IOWR(HARDWARE_ID, 0x24, char[HARDWARE_MAX_ITEM_LONGTH])     //depth
#define HARDWARE_BACK_CAM_EFUSEID_GET             _IOWR(HARDWARE_ID, 0x25, char[HARDWARE_MAX_ITEM_LONGTH])      //  后摄efuseId
#define HARDWARE_BCAK_SUBCAM_EFUSEID_GET          _IOWR(HARDWARE_ID, 0x26, char[HARDWARE_MAX_ITEM_LONGTH])      //  后副摄efuseId
#define HARDWARE_FRONT_CAME_EFUSEID_GET           _IOWR(HARDWARE_ID, 0x27, char[HARDWARE_MAX_ITEM_LONGTH])      //  前摄efuseId
#define HARDWARE_FRONT_SUBCAME_EFUSEID_GET        _IOWR(HARDWARE_ID, 0x28, char[HARDWARE_MAX_ITEM_LONGTH])      //  前副摄efuseId
#define HARDWARE_BACK_CAM_SENSORID_GET            _IOWR(HARDWARE_ID, 0x29, char[HARDWARE_MAX_ITEM_LONGTH])      //  后摄sensorId
#define HARDWARE_BACK_SUBCAM_SENSORID_GET         _IOWR(HARDWARE_ID, 0x30, char[HARDWARE_MAX_ITEM_LONGTH])      //  后副摄sensorId
#define HARDWARE_FRONT_CAM_SENSORID_GET           _IOWR(HARDWARE_ID, 0x31, char[HARDWARE_MAX_ITEM_LONGTH])      //  前摄sensorId
#define HARDWARE_FRONT_SUBCAM_SENSORID_GET        _IOWR(HARDWARE_ID, 0x32, char[HARDWARE_MAX_ITEM_LONGTH])      //  前副摄sensorId
//bug 349613,20180409,huwei2,add Sar in hardwareInfo
#define HARDWARE_SAR_GET                          _IOWR(HARDWARE_ID, 0x33, char[HARDWARE_MAX_ITEM_LONGTH])      //sar
#define HARDWARE_HALL_GET                          _IOWR(HARDWARE_ID, 0x1B, char[HARDWARE_MAX_ITEM_LONGTH])     //hall
#define HARDWARE_WIDE_ANGLE_CAM_GET                     _IOWR(HARDWARE_ID, 0x34, char[HARDWARE_MAX_ITEM_LONGTH])     //wide
#define HARDWARE_WIDE_ANGLE_CAM_MOUDULE_ID_GET          _IOWR(HARDWARE_ID, 0x35, char[HARDWARE_MAX_ITEM_LONGTH])      //wide
#define HARDWARE_WIDE_ANGLE_CAM_EFUSEID_GET             _IOWR(HARDWARE_ID, 0x36, char[HARDWARE_MAX_ITEM_LONGTH])      //  广角CAM efuseId
#define HARDWARE_WIDE_ANGLE_CAM_SENSORID_GET            _IOWR(HARDWARE_ID, 0x37, char[HARDWARE_MAX_ITEM_LONGTH])      //  广角CAM sensorId

#define HARDWARE_IR_CAM_GET                       _IOWR(HARDWARE_ID, 0x38, char[HARDWARE_MAX_ITEM_LONGTH])      //  红外CAM
#define HARDWARE_IR_CAM_MOUDULE_ID_GET            _IOWR(HARDWARE_ID, 0x39, char[HARDWARE_MAX_ITEM_LONGTH])      //  红外CAM模组厂
#define HARDWARE_IR_CAM_EFUSEID_GET               _IOWR(HARDWARE_ID, 0x3A, char[HARDWARE_MAX_ITEM_LONGTH])      //  红外CAM efuseId
#define HARDWARE_IR_CAM_SENSORID_GET              _IOWR(HARDWARE_ID, 0x3B, char[HARDWARE_MAX_ITEM_LONGTH])      //  红外CAM sensor

#define HARDWARE_MACRO_CAM_GET                    _IOWR(HARDWARE_ID, 0x3C, char[HARDWARE_MAX_ITEM_LONGTH])      //macro
#define HARDWARE_MACRO_CAM_MOUDULE_ID_GET         _IOWR(HARDWARE_ID, 0x3D, char[HARDWARE_MAX_ITEM_LONGTH])      //macro
#define HARDWARE_MACRO_CAM_EFUSEID_GET            _IOWR(HARDWARE_ID, 0x3E, char[HARDWARE_MAX_ITEM_LONGTH])      //  微距CAM efuseId
#define HARDWARE_MACRO_CAM_SENSORID_GET           _IOWR(HARDWARE_ID, 0x3F, char[HARDWARE_MAX_ITEM_LONGTH])      //  微距CAM sensorId
#define HARDWARE_SMARTPA_IC_GET                   _IOWR(HARDWARE_ID, 0x40, char[HARDWARE_MAX_ITEM_LONGTH])
#define HARDWARE_BMS_GAUGE_GET                     _IOWR(HARDWARE_ID, 0x41, char[HARDWARE_MAX_ITEM_LONGTH])      //Bug493560,lili5.wt,ADD,20191112,Add fuel gauge information in factory mode
#define HARDWARE_BACK_CAM_MOUDULE_ID_SET          _IOWR(HARDWARE_ID, 0x81, char[HARDWARE_MAX_ITEM_LONGTH])
#define HARDWARE_FRONT_CAM_MODULE_ID_SET          _IOWR(HARDWARE_ID, 0x82, char[HARDWARE_MAX_ITEM_LONGTH])
#define HARDWARE_SDCARD_STATUS                    _IOWR(HARDWARE_ID, 0x83, char[HARDWARE_MAX_ITEM_LONGTH])
#define HARDWARE_FINGER_GET                       _IOWR(HARDWARE_ID, 0x84, char[HARDWARE_MAX_ITEM_LONGTH])


/*+Bug 313110 -  guojunbo.wt;add;20171026;add for lg sku */
#define HARDWARE_SKU_INFO_GET                     _IOWR(HARDWARE_ID, 0x85, char[HARDWARE_MAX_ITEM_LONGTH])
#define HARDWARE_CHARGER_IC_INFO_GET              _IOWR(HARDWARE_ID, 0x86, char[HARDWARE_MAX_ITEM_LONGTH])

/*-Bug 313110 -  guojunbo.wt;add;20171026;add for lg sku */

#define HARDWARE_SECURE_INFO_GET                  _IOWR(HARDWARE_ID, 0x87, char[HARDWARE_MAX_ITEM_LONGTH])
#define SOFTWARE_SECURE_INFO_GET                  _IOWR(HARDWARE_ID, 0x88, char[HARDWARE_MAX_ITEM_LONGTH])

#define HARDWARE_CHARGER_IC_SLAVE_INFO_GET        _IOWR(HARDWARE_ID, 0x90, char[HARDWARE_MAX_ITEM_LONGTH])



int hardwareinfo_set_prop(int cmd, const char *name);

#endif //__HARDWARE_H__
