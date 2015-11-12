
#include "stdint.h"

typedef struct
{
  uint32_t RTC_HourFormat;   /*!< Specifies the RTC Hour Format.
                             This parameter can be a value of @ref RTC_Hour_Formats */
  
  uint32_t RTC_AsynchPrediv; /*!< Specifies the RTC Asynchronous Predivider value.
                             This parameter must be set to a value lower than 0x7F */
  
  uint32_t RTC_SynchPrediv;  /*!< Specifies the RTC Synchronous Predivider value.
                             This parameter must be set to a value lower than 0x1FFF */
}RTC_InitTypeDef;


typedef struct
{
  uint8_t RTC_Hours;    /*!< Specifies the RTC Time Hour.
                        This parameter must be set to a value in the 0-12 range
                        if the RTC_HourFormat_12 is selected or 0-23 range if
                        the RTC_HourFormat_24 is selected. */

  uint8_t RTC_Minutes;  /*!< Specifies the RTC Time Minutes.
                        This parameter must be set to a value in the 0-59 range. */
  
  uint8_t RTC_Seconds;  /*!< Specifies the RTC Time Seconds.
                        This parameter must be set to a value in the 0-59 range. */

  uint8_t RTC_H12;      /*!< Specifies the RTC AM/PM Time.
                        This parameter can be a value of @ref RTC_AM_PM_Definitions */
}RTC_TimeTypeDef; 


typedef struct
{
  RTC_TimeTypeDef RTC_AlarmTime;     /*!< Specifies the RTC Alarm Time members. */

  uint32_t RTC_AlarmMask;            /*!< Specifies the RTC Alarm Masks.
                                     This parameter can be a value of @ref RTC_AlarmMask_Definitions */

  uint32_t RTC_AlarmDateWeekDaySel;  /*!< Specifies the RTC Alarm is on Date or WeekDay.
                                     This parameter can be a value of @ref RTC_AlarmDateWeekDay_Definitions */
  
  uint8_t RTC_AlarmDateWeekDay;      /*!< Specifies the RTC Alarm Date/WeekDay.
                                     This parameter must be set to a value in the 1-31 range 
                                     if the Alarm Date is selected.
                                     This parameter can be a value of @ref RTC_WeekDay_Definitions 
                                     if the Alarm WeekDay is selected. */
}RTC_AlarmTypeDef;

#define RTC_HourFormat_24              ((uint32_t)0x00000000)

#define RTC_H12_AM                     ((uint8_t)0x00)
#define RTC_H12_PM                     ((uint8_t)0x40)
#define IS_RTC_H12(PM) (((PM) == RTC_H12_AM) || ((PM) == RTC_H12_PM))


/** @defgroup RTC_AlarmDateWeekDay_Definitions 
  * @{
  */ 
#define RTC_AlarmDateWeekDaySel_Date      ((uint32_t)0x00000000)  
#define RTC_AlarmDateWeekDaySel_WeekDay   ((uint32_t)0x40000000)  

#define IS_RTC_ALARM_DATE_WEEKDAY_SEL(SEL) (((SEL) == RTC_AlarmDateWeekDaySel_Date) || \
                                            ((SEL) == RTC_AlarmDateWeekDaySel_WeekDay))


#define RTC_AlarmMask_DateWeekDay         ((uint32_t)0x80000000)  


