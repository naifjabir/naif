#include <iostream>
#include <cmath>

float clampTo2ByteFloat(float value) {
    // Max representable value for a 2-byte float
    const float max2ByteFloat = (1.0f - std::pow(2.0f, -10)) * std::pow(2.0f, 15);

    // Clamp the value
    return std::fmin(value, max2ByteFloat);
}

int main() {
    float originalValue = 70000.0f;  // Some example value

    // Simulate a 2-byte float by clamping the 4-byte float
    float clampedValue = clampTo2ByteFloat(originalValue);

    std::cout << "Original value: " << originalValue << std::endl;
    std::cout << "Clamped value: " << clampedValue << std::endl;

    return 0;
}