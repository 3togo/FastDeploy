// Copyright (c) 2022 PaddlePaddle Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "fastdeploy/backends/paddle/paddle_backend.h"

namespace fastdeploy {
void ShareTensorFromCpu(paddle_infer::Tensor* tensor, FDTensor& fd_tensor) {
  std::vector<int> shape(fd_tensor.shape.begin(), fd_tensor.shape.end());
  tensor->Reshape(shape);
  if (fd_tensor.dtype == FDDataType::FP32) {
    tensor->ShareExternalData(static_cast<const float*>(fd_tensor.Data()),
                              shape, paddle_infer::PlaceType::kCPU);
    return;
  } else if (fd_tensor.dtype == FDDataType::INT32) {
    tensor->ShareExternalData(static_cast<const int32_t*>(fd_tensor.Data()),
                              shape, paddle_infer::PlaceType::kCPU);
    return;
  } else if (fd_tensor.dtype == FDDataType::INT64) {
    tensor->ShareExternalData(static_cast<const int64_t*>(fd_tensor.Data()),
                              shape, paddle_infer::PlaceType::kCPU);
    return;
  }
  FDASSERT(false, "Unexpected data type(" + Str(fd_tensor.dtype) +
                      ") while infer with PaddleBackend.");
}

void CopyTensorToCpu(std::unique_ptr<paddle_infer::Tensor>& tensor,
                     FDTensor* fd_tensor) {
  auto fd_dtype = PaddleDataTypeToFD(tensor->type());
  fd_tensor->Allocate(tensor->shape(), fd_dtype, tensor->name());
  if (fd_tensor->dtype == FDDataType::FP32) {
    tensor->CopyToCpu(static_cast<float*>(fd_tensor->MutableData()));
    return;
  } else if (fd_tensor->dtype == FDDataType::INT32) {
    tensor->CopyToCpu(static_cast<int32_t*>(fd_tensor->MutableData()));
    return;
  } else if (fd_tensor->dtype == FDDataType::INT64) {
    tensor->CopyToCpu(static_cast<int64_t*>(fd_tensor->MutableData()));
    return;
  }
  FDASSERT(false, "Unexpected data type(" + Str(fd_tensor->dtype) +
                      ") while infer with PaddleBackend.");
}

FDDataType PaddleDataTypeToFD(const paddle_infer::DataType& dtype) {
  auto fd_dtype = FDDataType::FP32;
  if (dtype == paddle_infer::FLOAT32) {
    fd_dtype = FDDataType::FP32;
  } else if (dtype == paddle_infer::INT64) {
    fd_dtype = FDDataType::INT64;
  } else if (dtype == paddle_infer::INT32) {
    fd_dtype = FDDataType::INT32;
  } else if (dtype == paddle_infer::UINT8) {
    fd_dtype = FDDataType::UINT8;
  } else {
    FDASSERT(false, "Unexpected data type:" + std::to_string(int(dtype)) +
                        " while call CopyTensorToCpu in PaddleBackend.");
  }
  return fd_dtype;
}

}  // namespace fastdeploy