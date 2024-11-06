#include "nndeploy/op/expr.h"

namespace nndeploy {
namespace op {

Expr::~Expr(){};

Expr::Expr(const std::string &name) : expr_type_(kExprTypeValueDesc) {
  value_desc_ = std::make_shared<ir::ValueDesc>(name);
}
Expr::Expr(const std::string &name, base::DataType data_type)
    : expr_type_(kExprTypeValueDesc) {
  value_desc_ = std::make_shared<ir::ValueDesc>(name, data_type);
}
Expr::Expr(const std::string &name, base::DataType data_type,
           base::IntVector shape)
    : expr_type_(kExprTypeValueDesc) {
  value_desc_ = std::make_shared<ir::ValueDesc>(name, data_type, shape);
}
Expr::Expr(std::shared_ptr<ir::ValueDesc> value_desc)
    : expr_type_(kExprTypeValueDesc), value_desc_(value_desc) {}
Expr::Expr(std::shared_ptr<ir::OpDesc> op_desc)
    : expr_type_(kExprTypeOpDesc), op_desc_(op_desc) {}
Expr::Expr(std::shared_ptr<ir::ModelDesc> model_desc)
    : expr_type_(kExprTypeOpDesc), model_desc_(model_desc) {}

std::vector<std::string> Expr::getOutputName() {
  switch (expr_type_) {
    case kExprTypeValueDesc:
      return {value_desc_->name_};
    case kExprTypeOpDesc:
      return op_desc_->outputs_;
    case kExprTypeModelDesc: {
      std::vector<std::string> outputs;
      for (auto &output : model_desc_->outputs_) {
        outputs.push_back(output->name_);
      }
      return outputs;
    }
    default:
      return {""};
  }
}

std::shared_ptr<Expr> makeInput(ir::ModelDesc *model_desc, std::string name,
                                base::DataType data_type,
                                base::IntVector shape) {
  auto value_desc = std::make_shared<ir::ValueDesc>(name, data_type, shape);
  if (model_desc != nullptr) {
    model_desc->inputs_.push_back(value_desc);
  }
  auto expr = std::make_shared<Expr>(value_desc);
  return expr;
}
void makeOutput(ir::ModelDesc *model_desc, std::shared_ptr<Expr> expr) {
  if (model_desc != nullptr) {
    std::vector<std::string> output = expr->getOutputName();
    for (auto &name : output) {
      auto value_desc = std::make_shared<ir::ValueDesc>(name);
      model_desc->outputs_.push_back(value_desc);
    }
  }
}
std::shared_ptr<Expr> makeBlock(ir::ModelDesc *model_desc,
                                std::shared_ptr<ir::ModelDesc> model_block) {
  if (model_desc != nullptr) {
    model_desc->blocks_.push_back(model_block);
  }
  auto expr = std::make_shared<Expr>(model_block);
  return expr;
}
std::shared_ptr<Expr> makeConv(ir::ModelDesc *model_desc,
                               std::shared_ptr<Expr> input,
                               std::shared_ptr<ir::ConvParam> param,
                               const std::string &weight,
                               const std::string &bias, std::string op_name,
                               std::string output_name) {
  std::string name = op_name;
  if (name.empty()) {
    if (model_desc != nullptr) {
      int index = model_desc->op_descs_.size();
      name = "conv" + std::to_string(index);
    } else {
      name = "conv";
    }
  }
  std::vector<std::string> inputs = {input->getOutputName()[0]};
  if (!weight.empty()) {
    inputs.push_back(weight);
  }
  if (!bias.empty()) {
    inputs.push_back(bias);
  }
  // 节点输出
  std::vector<std::string> outputs;
  if (!output_name.empty()) {
    outputs.push_back(output_name);
  } else {
    outputs.push_back(name + ".output");
  }
  auto op_desc = std::make_shared<ir::OpDesc>(name, ir::kOpTypeConv, inputs,
                                              outputs, param);
  if (model_desc != nullptr) {
    model_desc->op_descs_.push_back(op_desc);
  }

  auto expr = std::make_shared<Expr>(op_desc);
  return expr;
}

std::shared_ptr<Expr> makeSoftMax(ir::ModelDesc *model_desc,
                                  std::shared_ptr<Expr> input,
                                  std::shared_ptr<ir::SoftmaxParam> param,
                                  std::string op_name,
                                  std::string output_name) {
  std::string name = op_name;
  if (name.empty()) {
    if (model_desc != nullptr) {
      int index = model_desc->op_descs_.size();
      name = "softmax" + std::to_string(index);
    } else {
      name = "softmax";
    }
  }
  std::vector<std::string> inputs = {input->getOutputName()[0]};
  // 节点输出
  std::vector<std::string> outputs;
  if (!output_name.empty()) {
    outputs.push_back(output_name);
  } else {
    outputs.push_back(name + ".output");
  }
  auto op_desc = std::make_shared<ir::OpDesc>(name, ir::kOpTypeSoftmax, inputs,
                                              outputs, param);
  if (model_desc != nullptr) {
    model_desc->op_descs_.push_back(op_desc);
  }

  auto expr = std::make_shared<Expr>(op_desc);
  return expr;
}

std::shared_ptr<Expr> makeAdd(ir::ModelDesc *model_desc,
                              std::shared_ptr<Expr> input_0,
                              std::shared_ptr<Expr> input_1,
                              std::string op_name, std::string output_name) {
  std::string name = op_name;
  if (name.empty()) {
    if (model_desc != nullptr) {
      int index = model_desc->op_descs_.size();
      name = "add" + std::to_string(index);
    } else {
      name = "add";
    }
  }
  std::vector<std::string> inputs;
  inputs.emplace_back(input_0->getOutputName()[0]);
  inputs.emplace_back(input_1->getOutputName()[0]);
  // 节点输出
  std::vector<std::string> outputs;
  if (!output_name.empty()) {
    outputs.push_back(output_name);
  } else {
    outputs.push_back(name + ".output");
  }
  auto op_desc =
      std::make_shared<ir::OpDesc>(name, ir::kOpTypeAdd, inputs, outputs);
  if (model_desc != nullptr) {
    model_desc->op_descs_.push_back(op_desc);
  }

  auto expr = std::make_shared<Expr>(op_desc);
  return expr;
}

std::shared_ptr<Expr> makeRelu(ir::ModelDesc *model_desc,
                               std::shared_ptr<Expr> input, std::string op_name,
                               std::string output_name) {
  std::string name = op_name;
  if (name.empty()) {
    if (model_desc != nullptr) {
      int index = model_desc->op_descs_.size();
      name = "relu" + std::to_string(index);
    } else {
      name = "relu";
    }
  }
  std::vector<std::string> inputs = {input->getOutputName()[0]};
  // 节点输出
  std::vector<std::string> outputs;
  if (!output_name.empty()) {
    outputs.push_back(output_name);
  } else {
    outputs.push_back(name + ".output");
  }
  auto op_desc =
      std::make_shared<ir::OpDesc>(name, ir::kOpTypeRelu, inputs, outputs);
  if (model_desc != nullptr) {
    model_desc->op_descs_.push_back(op_desc);
  }

  auto expr = std::make_shared<Expr>(op_desc);
  return expr;
}

// batchnorm
NNDEPLOY_CC_API std::shared_ptr<Expr> makeBatchNorm(
    ir::ModelDesc *model_desc, std::shared_ptr<Expr> input,
    std::shared_ptr<ir::BatchNormalizationParam> param,
    const std::string &scale, const std::string &bias, const std::string &mean,
    const std::string &var, std::string op_name, std::string output_name) {
  std::string name = op_name;
  if (name.empty()) {
    if (model_desc != nullptr) {
      int index = model_desc->op_descs_.size();
      name = "batchnorm" + std::to_string(index);
    } else {
      name = "batchnorm";
    }
  }
  std::vector<std::string> inputs = {input->getOutputName()[0]};
  if (!scale.empty()) {
    inputs.push_back(scale);
  }
  if (!bias.empty()) {
    inputs.push_back(bias);
  }
  if (!mean.empty()) {
    inputs.push_back(mean);
  }
  if (!bias.empty()) {
    inputs.push_back(var);
  }

  std::vector<std::string> outputs;
  if (!output_name.empty()) {
    outputs.push_back(output_name);
  } else {
    outputs.push_back(name + ".output");
  }
  auto op_desc = std::make_shared<ir::OpDesc>(
      name, ir::kOpTypeBatchNormalization, inputs, outputs, param);
  if (model_desc != nullptr) {
    model_desc->op_descs_.push_back(op_desc);
  }

  auto expr = std::make_shared<Expr>(op_desc);
  return expr;
}

}  // namespace op
}  // namespace nndeploy