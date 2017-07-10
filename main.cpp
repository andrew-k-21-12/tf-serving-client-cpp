#include <grpc++/grpc++.h>
#include <opencv2/opencv.hpp>

#include "tensorflow_serving/apis/prediction_service.grpc.pb.h"



// Constants.
#define HOST "yourhost.com"
#define PORT 9000
#define MODEL_NAME "default"
#define MODEL_SIGNATURE_NAME "predict"
#define MODEL_INPUT "images"



/// Type alias for string-TensorProto map.
typedef google::protobuf::Map<std::string, tensorflow::TensorProto> StringKeyedProtos;



/// Entry point.
int main(int argc, char** argv) {

  // Checking args.
  if (argc < 2) {
    std::cerr << "Please provide a path to the image to be recognized via TensorFlow server" << std::endl;
    return 1;
  }

  // Opening an image.
  const char* imagePath = argv[1];
  cv::Mat image = cv::imread(imagePath);
  if (image.empty()) {
    std::cerr << "Could not open provided image: " << imagePath << std::endl;
    return 1;
  }



  // Preparing required variables to make a predict request.
  tensorflow::serving::PredictRequest predictRequest;
  tensorflow::serving::PredictResponse response;
  grpc::ClientContext context;

  // Describing model name and signature from remote server.
  predictRequest.mutable_model_spec()->set_name(MODEL_NAME);
  predictRequest.mutable_model_spec()->set_signature_name(MODEL_SIGNATURE_NAME);



  // Describing remote model inputs shape.
  StringKeyedProtos& inputs = *predictRequest.mutable_inputs();

  // Setting dimensions of the input shape.
  tensorflow::TensorProto inputShape;
  inputShape.set_dtype(tensorflow::DataType::DT_FLOAT);
  inputShape.mutable_tensor_shape()->add_dim()->set_size(1);                // one image
  inputShape.mutable_tensor_shape()->add_dim()->set_size(image.cols);       // with its image size
  inputShape.mutable_tensor_shape()->add_dim()->set_size(image.rows);
  inputShape.mutable_tensor_shape()->add_dim()->set_size(image.channels()); // and its channels count



  // Loading an image for the request.
  for (auto x = 0; x < image.cols; ++x)
    for (auto y = 0; y < image.rows; ++y) {
      cv::Vec3b intensity = image.at<cv::Vec3b>(x, y);
      for (auto c = 0; c < image.channels(); ++c) {
        inputShape.add_float_val((float) intensity.val[c]);
      }
    }
  inputs[MODEL_INPUT] = inputShape;



  // Preparing request executor.
  std::unique_ptr<tensorflow::serving::PredictionService::Stub> stub =
          tensorflow::serving::PredictionService::NewStub(
                  grpc::CreateChannel(
                          std::string(HOST) + ":" + std::to_string(PORT), grpc::InsecureChannelCredentials()));

  // Firing predict request.
  grpc::Status status = stub->Predict(&context, predictRequest, &response);



  // Checking server response status.
  if (!status.ok()) {
    std::cerr << "Predict request has failed with code " << status.error_code()
              << " and message: " << status.error_message() << std::endl;
    return 1;
  }

  // Iterating through results.
  StringKeyedProtos& outputs = *response.mutable_outputs();
  StringKeyedProtos::iterator iter;
  for (iter = outputs.begin(); iter != outputs.end(); ++iter) {
    tensorflow::TensorProto& output = iter->second;

    // Looking for the max predictor's confidence.
    float max = -FLT_MAX;
    int maxIndex = -1;
    for (auto i = 0; i < output.float_val_size(); ++i) {
      const float outValue = output.float_val(i);
      if (outValue > max) {
        max = outValue;
        maxIndex = i;
      }
    }

    // Showing result from remote server.
    if (maxIndex != -1) {
      std::cout << "Result class is " << maxIndex << " with response " << max << std::endl;
    }
  }



  return 0;
}
