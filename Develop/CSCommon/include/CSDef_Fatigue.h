#pragma once

#define DEC_FATIGUE_PERIOD 60.0f						// 피로도 감소 주기

#define DEC_FATIGUE_PER_MINUTE_ON_NOT_HUNTING_GROUND 0	// 비사냥터에 있을때 분당 감소 피로도
#define DEC_FATIGUE_PER_MINUTE_ON_HUNTING_GROUND 0		// 사냥터에 있을때 분당 감소 피로도
#define DEC_FATIGUE_PER_MINUTE_ON_HUNTING_GROUND 0		// 사냥터에 있을때 분당 감소 피로도

#define INC_FATIGUE_PER_HOUR_BY_BLANKTIME 0	// 접속 안하고 있을때 시간당 증가 피로도

#define SET_FATIGUE_ROOM_NORMAL 3000	// 일반 방에서 잘때 바뀌는 피로도
#define SET_FATIGUE_ROOM_SPECIAL 3000	// 특별한 방에서 잘때 바뀌는 피로도


//#define DEC_FATIGUE_PER_MINUTE_ON_NOT_HUNTING_GROUND 20	// 비사냥터에 있을때 분당 감소 피로도
//#define DEC_FATIGUE_PER_MINUTE_ON_HUNTING_GROUND 30		// 사냥터에 있을때 분당 감소 피로도
//#define DEC_FATIGUE_PER_MINUTE_ON_HUNTING_GROUND 30		// 사냥터에 있을때 분당 감소 피로도

//#define INC_FATIGUE_PER_HOUR_BY_BLANKTIME 500	// 접속 안하고 있을때 시간당 증가 피로도
//
//#define SET_FATIGUE_ROOM_NORMAL 4000	// 일반 방에서 잘때 바뀌는 피로도
//#define SET_FATIGUE_ROOM_SPECIAL 4700	// 특별한 방에서 잘때 바뀌는 피로도

#define WORST_FATIGUE		0
#define BAD_FATIGUE			1000
#define NORMAL_FATIGUE		3000
#define GOOD_FATIGUE		4100
#define BEST_FATIGUE		5000
