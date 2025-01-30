#pragma once

class Random {
public: 
    
    static double nextDouble(double min, double max);
    static float nextFloat(float min, float max);
    static int nextInt(int min, int max);
    static void randomDelay(int maxMs = 1000);
private:
    
};