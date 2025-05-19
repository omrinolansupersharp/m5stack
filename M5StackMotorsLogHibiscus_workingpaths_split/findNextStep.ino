int findNextStep(const float steps[], int size, float currentPosition){
    int nextStepIndex = -1;
    float nextStep = std::numeric_limits<float>::max(); // Declare and initialize nextStep
    for (int i = 0; i < size; i++) {
        if (steps[i] > currentPosition && steps[i] < nextStep) {
            nextStep = steps[i]; // Update nextStep
            nextStepIndex = i;
        }
    }
    return nextStepIndex;
}