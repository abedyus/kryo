#include "FOnnxInferenceSession.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"
#include "Logging/LogMacros.h"

FOnnxInferenceSession::FOnnxInferenceSession()
{
    UE_LOG(LogTemp, Log, TEXT("FOnnxInferenceSession Constructor Called"));

    try
    {
        // Initialize the ONNX Runtime environment
        Env = MakeUnique<Ort::Env>(ORT_LOGGING_LEVEL_WARNING, "OnnxRuntime");
        UE_LOG(LogTemp, Log, TEXT("ONNX Env created successfully."));

        // Initialize the SessionOptions object
        SessionOptions = Ort::SessionOptions();
        SessionOptions.SetIntraOpNumThreads(1); // Configure the number of threads for CPU execution
    }
    catch (const Ort::Exception& e)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to initialize ONNX Runtime: %s"), UTF8_TO_TCHAR(e.what()));
        // Handle the error appropriately (e.g., throw an exception or set a flag)
    }
}

FOnnxInferenceSession::~FOnnxInferenceSession()
{
    // Cleanup is handled automatically.
}

bool FOnnxInferenceSession::Initialize(const FString& ModelPath)
{
    if (!Env.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("ONNX Runtime environment is not initialized."));
        return false;
    }

    UE_LOG(LogTemp, Log, TEXT("Initializing ONNX Runtime Session..."));

    // Log the absolute path
    FString AbsolutePath = IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*ModelPath);
    UE_LOG(LogTemp, Log, TEXT("Absolute model path: %s"), *AbsolutePath);
    if (!FPaths::FileExists(AbsolutePath))
    {
        UE_LOG(LogTemp, Error, TEXT("ONNX model not found: %s"), *AbsolutePath);
        return false;
    }

    std::wstring ModelPathW(*AbsolutePath);
    UE_LOG(LogTemp, Log, TEXT("Creating ONNX Session using model: %s"), *AbsolutePath);

    try
    {
        // Create the ONNX session
        Session = MakeUnique<Ort::Session>(*Env, ModelPathW.c_str(), SessionOptions);
        UE_LOG(LogTemp, Log, TEXT("ONNX Session created successfully."));
    }
    catch (const Ort::Exception& e)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create ONNX session: %s"), UTF8_TO_TCHAR(e.what()));
        return false;
    }

    return true;
}

bool FOnnxInferenceSession::RunInference(const TArray<float>& InputData, TArray<float>& OutputData)
{
    if (!Session.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("ONNX Session is not initialized."));
        return false;
    }

    // Create CPU memory info for the tensor.
    Ort::MemoryInfo MemoryInfo = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);

    // Define input tensor dimensions.
    // This example assumes a 2D tensor with shape [1, N] where N = InputData.Num().
    std::vector<int64_t> InputDims = { 1, static_cast<int64_t>(InputData.Num()) };

    // Create the input tensor from InputData.
    Ort::Value InputTensor = Ort::Value::CreateTensor<float>(
        MemoryInfo,
        const_cast<float*>(InputData.GetData()),
        InputData.Num(),
        InputDims.data(),
        InputDims.size());

    // The following input and output names must match your model's graph.
    // Update these names as appropriate.
    const char* InputNames[] = { "input" };
    const char* OutputNames[] = { "output" };

    try
    {
        // Run inference.
        auto OutputTensors = Session->Run(
            Ort::RunOptions{ nullptr },
            InputNames,
            &InputTensor,
            1,
            OutputNames,
            1);

        // Assume the output is a tensor of floats.
        float* OutputTensorData = OutputTensors.front().GetTensorMutableData<float>();

        // Get the number of elements in the output tensor.
        size_t OutputCount = OutputTensors.front().GetTensorTypeAndShapeInfo().GetElementCount();

        // Resize the output array and copy the output data.
        OutputData.SetNumUninitialized(OutputCount);
        FMemory::Memcpy(OutputData.GetData(), OutputTensorData, OutputCount * sizeof(float));
    }
    catch (const Ort::Exception& e)
    {
        UE_LOG(LogTemp, Error, TEXT("Inference error: %s"), UTF8_TO_TCHAR(e.what()));
        return false;
    }

    return true;
}