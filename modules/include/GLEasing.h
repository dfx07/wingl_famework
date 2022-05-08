//********************************************************************************//
//    Chức năng tính toán các hiệu ứng easing (animation)                         //
//    Author: Thuong.NV         Date : 6/5/2022                                   //
//********************************************************************************//

//==================================================================================
// Chỉ nên sử dụng nó với một số chức năng cơ bản, đối với các hiện tượng vật lý nên
// sử dụng các thành phần vật lý để tạo ra cảm giác thật                            
//==================================================================================


#ifndef GLEASING_H
#define GLEASING_H

#include <iostream>

#define EASING_PI               3.14159265359f
#define EASING_EPSILON          0.001f

#define EASING_STANDTIME_START  0.f
#define EASING_STANDTIME_END    1.f

/*
Easing functions : specify the rate of change of a parameter over time.             

Bao gồm tính toán các hiệu ứng easing:
        +   Back                                                                    
        +   Quint                                                                   
        +   Elastic                                                                 
        +   Quart                                                                   

Các phương thức tính toán này dựa vào toán học và thường được áp dụng vào vị trí    

Option:
    EaseIn   : Interpolation follows the mathematical formula associated with the   
               easing function.                                                     
    EaseOut  : Interpolation follows 100% interpolation minus the output of the     
               formula associated with the easing function.                         
    EaseInOut: Interpolation uses EaseIn for the first half of the animation and    
               EaseOut for the second half.                                         
*/


enum class EaseMode
{
    In,
    Out,
    InOut
};

enum class EaseType
{
    Back   ,
    Quint  ,
    Elastic,
    Quart  ,
};
//==================================================================================
//⮟⮟ Hàm bổ trợ                                                                    
//==================================================================================

static float EasingClamp(float x, float min, float max)
{
    return (x <= min) ? min : (x >= max ? max : x);
}

static float EasingHardMap(float x, float min_s, float max_s, float min_d, float max_d)
{
    if (fabs(max_s - min_s) <= EASING_EPSILON) return min_d;

    float value = ((x - min_s)*(max_d - min_d)) / (max_s - min_s) + min_d;
    return EasingClamp(value, min_d, max_d);
}

static float EasingSoftMap(float x, float min_s, float max_s, float min_d, float max_d)
{
    float A = (max_s - x);
    float B = (min_s - x);

    if      (fabs(A) <= EASING_EPSILON) return max_d;
    else if (fabs(B) <= EASING_EPSILON) return min_d;

    if (fabs(A - B) <= EASING_EPSILON)
    {
        return min_d;
    }

    float value = (A*min_d - B*max_d) / (A - B);
    return value;
}

//==================================================================================
//⮟⮟ Triển khai hàm  EASING BACK //t : 0->1                                        
//==================================================================================
static float EaseInBack(float t)
{
    const float c1 = 1.70158f;
    const float c3 = c1 + 1;

    return c3*t*t*t - c1*t*t;
}

static float EaseOutBack(float t)
{
    const float c1 = 1.70158f;
    const float c3 = c1 + 1;

    return 1 + c3 * std::powf(t - 1, 3) + c1 * std::powf(t - 1, 2);
}

static float EaseInOutBack(float t)
{
    const float c1 = 1.70158f;
    const float c2 = c1 * 1.525f;

    return t < 0.5
        ? (std::powf(2 * t, 2) * ((c2 + 1) * 2 * t - c2)) / 2
        : (std::powf(2 * t - 2, 2) * ((c2 + 1) * (t * 2 - 2) + c2) + 2) / 2;
}

//==================================================================================
//⮟⮟ Triển khai hàm  EASING Quint //t : 0->1                                       
//==================================================================================
static float EaseInQuint(float t)
{
    return t * t * t * t * t;
}

static float EaseOutQuint(float t)
{
    return 1 - std::powf(1 - t, 5);
}

static float EaseInOutQuint(float t)
{
    return t < 0.5 ? 16 * t * t * t * t * t : 1 - std::powf(-2 * t + 2, 5) / 2;
}


//==================================================================================
//⮟⮟ Triển khai hàm  EASING Elastic //t : 0->1                                       
//==================================================================================
static float EaseInElastic(float t)
{
    const float c4 = (2 * EASING_PI) / 3.f;

    return t == 0
        ? 0
        : t == 1
        ? 1
        : -std::powf(2, 10 * t - 10) * sin((t * 10 - 10.75f) * c4);
}

static float EaseOutElastic(float t)
{
    const float c4 = (2 * EASING_PI) / 3.f;

    return t == 0
        ? 0
        : t == 1
        ? 1
        : std::powf(2, -10 * t) * sin((t * 10 - 0.75f) * c4) + 1;
}

static float EaseInOutElastic(float t)
{
    const float c5 = (2 * EASING_PI) / 4.5f;

    return t == 0
        ? 0
        : t == 1
        ? 1
        : t < 0.5
        ? -(std::powf(2,  20 * t - 10) * sin((20 * t - 11.125f) * c5)) / 2
        :  (std::powf(2, -20 * t + 10) * sin((20 * t - 11.125f) * c5)) / 2 + 1;
}

//==================================================================================
//⮟⮟ Triển khai hàm  EASING Quart //t : 0->1                                       
//==================================================================================
static float EaseInQuart(float t)
{
    return t * t * t * t;
}

static float EaseOutQuart(float t)
{
    return 1 - std::powf(1 - t, 4);
}

static float EaseInOutQuart(float t)
{
    return t < 0.5 ? 8 * t * t * t * t : 1 - std::powf(-2 * t + 2, 4) / 2;
}


// Return : Giá trị trong khoảng [vfrom -> vto] tại thời điểm t
static float CallEasingBack(EaseMode  mode      ,// Chế độ
                            float     t         ,// Thời điểm t từ lúc tính
                            float     vfrom     ,// Giá trị bắt đầu
                            float     vto       ,// Giá trị kết thúc
                            float     duration)  // Khoảng thời gian diễn ra
{
    if (t >= duration) return vto;

    float t1   = EasingHardMap(t, 0.f, duration, EASING_STANDTIME_START, EASING_STANDTIME_END);

    float vt1 = 0.f;
    if (mode == EaseMode::Out)
    {
        vt1 = EaseOutBack(t1);
    }
    else if (mode == EaseMode::InOut)
    {
        vt1 = EaseInOutBack(t1);
    }
    else
    {
        vt1 = EaseInBack(t1);
    }

    float value = EasingSoftMap(vt1, EASING_STANDTIME_START, EASING_STANDTIME_END, vfrom, vto);
    return value;
}

#endif // !GLEASING_H
