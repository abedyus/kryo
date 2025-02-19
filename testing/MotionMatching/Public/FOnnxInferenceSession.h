#pragma once

#include "CoreMinimal.h"
// Include the official ONNX Runtime C++ API header.
// (This should resolve via the ThirdParty include path set in Build.cs.)
#include "onnxruntime_cxx_api.h"

/**
 * A simple wrapper for ONNX Runtime inference.
 */
class TESTING_API FOnnxInferenceSession
{
public:
    FOnnxInferenceSession();
    ~FOnnxInferenceSession();

    /**
     * Initializes the ONNX Runtime session with the provided model file.
     * @param ModelPath Full path to the ONNX model.
     * @return true if the session was successfully created.
     */
    bool Initialize(const FString& ModelPath);

    /**
     * Runs inference on the given input feature vector.
     * @param InputData The input features as a TArray<float>.
     * @param OutputData (Out) The model's output data.
     * @return true if inference succeeded.
     */
    bool RunInference(const TArray<float>& InputData, TArray<float>& OutputData);

private:
    // Delay the creation of the ONNX Runtime environment until Initialize() is called.
    TUniquePtr<Ort::Env> Env;

    // Unique pointer to the ONNX session.
    TUniquePtr<Ort::Session> Session;

    // Session options (configured in the constructor or Initialize).
    Ort::SessionOptions SessionOptions;
};