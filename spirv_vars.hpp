/*
 Copyright 2017 Google Inc.
 
 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at
 
 http://www.apache.org/licenses/LICENSE-2.0
 
 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
*/

#ifndef __SPIRV_VARS_HPP_INCLUDED__
#define __SPIRV_VARS_HPP_INCLUDED__

#include <cstdint>
#include <fstream>
#include <map>
#include <set>
#include <vector>

#include <vulkan/spirv.hpp>
#include <vulkan/vulkan.h>

namespace spirv_vars {

enum Error {
  ERROR_NONE  = 0,
  ERROR_UNEXPECTED_EOF,
  ERROR_INVALID_OPCODE,
  ERROR_INCOMPLETE_INSTRUCTION,
  ERROR_INCOMPLETE_DECORATION,
  ERROR_DUPLICATE_SHADER_STAGE,
  ERROR_TYPE_RESOLVE_FAILED,
};

enum {
  SHADER_STAGE_COUNT = 6
};

template <typename EnumT>
EnumT InvalidEnum(uint32_t base_value = UINT32_MAX) {
  EnumT res = static_cast<EnumT>(base_value);
  return res;
}

//! \struct SpirvHeader
//!
//!
struct SpirvHeader {
  uint32_t  magic_number;
  uint32_t  version_number;
  uint32_t  generator_magic_number;
  uint32_t  bound;
  uint32_t  reserved;
};

//! \struct TraitsInt
//!
//!
struct TraitsInt {
  uint32_t  width;
  uint32_t  signedness;
};

//! \struct TraitsFloat
//!
//!
struct TraitsFloat {
  uint32_t  width;
};

//! \struct TraitsVector
//!
//!
struct TraitsVector {
  uint32_t  component_type_id;
  uint32_t  component_count;
};

//! \struct TraitsMatrix
//!
//!
struct TraitsMatrix {
  uint32_t  column_type_id;
  uint32_t  column_count;
  uint32_t  row_count;
};

//! \struct TraitsImage
//!
//!
struct TraitsImage {
  uint32_t          sampled_type_id;
  spv::Dim          dim;
  uint32_t          depth;
  uint32_t          arrayed;
  uint32_t          multi_sample;
  uint32_t          sampled;
  spv::ImageFormat  image_format;
  uint32_t          access_qualifier;
};

//! \struct TraitsSampledImage
//!
//!
struct TraitsSampledImage {
  uint32_t  image_type;
};

//! \struct TraitsArray
//!
//!
struct TraitsArray {
  uint32_t  element_type_id;
  uint32_t  length_id;
};

//! \struct TraitsRuntimeArray
//!
//!
struct TraitsRuntimeArray {
  uint32_t  element_type;
};

//! \struct StructMember
//!
//!
struct StructMember {
  uint32_t      index;
  std::string   name;
  uint32_t      type_id = 0;
  uint32_t      location = UINT32_MAX;
  uint32_t      binding = UINT32_MAX;
  uint32_t      offset = 0;
};

//! \struct Result
//! 
//!
struct Result {
  uint32_t              id = 0;
  std::string           name;
  // op_type should only have OpType* opcode as the value
  spv::Op               op_type = InvalidEnum<spv::Op>();
  spv::StorageClass     storage_class = InvalidEnum<spv::StorageClass>();
  uint32_t              type_id = 0;
  // Traits
  union {
    TraitsInt           traits_int;
    TraitsFloat         traits_float;
    TraitsVector        traits_vector;
    TraitsMatrix        traits_matrix;
    TraitsImage         traits_image = {};
    TraitsSampledImage  traits_sampled_image;
    TraitsArray         traits_array;
    TraitsRuntimeArray  traits_runtime_array;
  };
  // OpTypeStruct
  std::map<uint32_t, StructMember>  struct_members;
  // OpConstant
  std::vector<uint32_t> constant_values;
  // Location, Set, Binding
  uint32_t              location = UINT32_MAX;
  uint32_t              descriptor_set = UINT32_MAX;
  uint32_t              binding = UINT32_MAX;
  uint32_t              offset = 0;
  // block applies to non-SSBO interface blocks
  bool                  block = false;
  // buffer_block applies to SSBO interface blocks
  bool                  buffer_block = false;
  // OpLoad, OpStore, OpAccessChain 
  bool                  accessed = false;
};

//! \struct DescriptorBinding
//!
//!
struct DescriptorBindingDesc : VkDescriptorSetLayoutBinding {
  std::string name;
};

//! \struct DescriptorSet
//!
//!
struct DescriptorSetDesc {
  uint32_t                            set = UINT32_MAX;
  std::vector<DescriptorBindingDesc>  binding_descs;
};

//!\ struct Spirv
//!
//!
struct Spirv {
  spv::ExecutionModel             execution_model = InvalidEnum<spv::ExecutionModel>();
  uint32_t                        entry_point_id = 0;
  std::string                     name;
  std::vector<uint32_t>           interfaces;
  std::map<uint32_t, Result>      results;
  std::vector<const Result*>      inputs;
  std::vector<const Result*>      outputs;
  std::vector<DescriptorSetDesc>  descriptor_set_descs;
};

//! \class Parser
//!
//!
class Parser {
public:
  Parser();
  Parser(const std::vector<uint32_t>& spirv_binary_0,
         const std::vector<uint32_t>& spirv_binary_1 = std::vector<uint32_t>(),
         const std::vector<uint32_t>& spirv_binary_2 = std::vector<uint32_t>(),
         const std::vector<uint32_t>& spirv_binary_3 = std::vector<uint32_t>(),
         const std::vector<uint32_t>& spirv_binary_4 = std::vector<uint32_t>(),
         const std::vector<uint32_t>& spirv_binary_5 = std::vector<uint32_t>());
  Parser(const std::string& file_path_0, 
         const std::string& file_path_1 = "", 
         const std::string& file_path_2 = "", 
         const std::string& file_path_3 = "", 
         const std::string& file_path_4 = "", 
         const std::string& file_path_5 = "");
  virtual ~Parser();

  bool IsValid() const { return m_error == ERROR_NONE; }
  std::vector<const DescriptorSetDesc*> GetDescriptorSets(spv::ExecutionModel stage) const;
  
protected:
  void ParseFinalize();
  void ParseSpirv(const std::string& file_path);
  void ParseSpirv(const std::vector<uint32_t>& spirv_binary);
  void ParseInstruction(spv::Op instr_opcode, 
                        uint32_t instr_word_count, 
                        const uint32_t* instr_words);
  void ParseDecoration(uint32_t target_id, 
                       uint32_t member_index,
                       spv::Decoration decoration,
                       const std::vector<uint32_t>& words);
  void ParseInputsAndOutputs(Spirv* spirv);
  void ParseDescriptorSets(Spirv* spirv);
  const Result* ResolveType(const Result* result, const Spirv* spirv) const;

  bool          has_next_operand() const;
  int32_t       parse_operand_int();
  uint32_t      parse_operand_uint();
  std::string   parse_operand_string();

protected:
  Error                       m_error = ERROR_NONE;
  std::vector<std::string>    m_msgs;
  std::vector<Spirv>          m_spirv;
  std::set<spv::Op>           m_unhandled_opcodes;
  std::set<spv::Decoration>   m_unhandled_decorations;
  
  const Spirv*  m_vert_spirv = nullptr;
  const Spirv*  m_tesc_spirv = nullptr;
  const Spirv*  m_tese_spirv = nullptr;
  const Spirv*  m_geom_spirv = nullptr;
  const Spirv*  m_frag_spirv = nullptr;
  const Spirv*  m_comp_spirv = nullptr;

  struct InstructionParse {
    uint32_t        instr_word_count = 0;
    const uint32_t* instr_words = nullptr;
    uint32_t        instr_word_index = 0;
  };
  InstructionParse  m_ip = {};
  Spirv*            m_cs = nullptr;
};

} // namespace spirv_vars


#if defined(SPIRV_VARS_IMPLEMENTATION)
#include <algorithm>
#include <sstream>

namespace spirv_vars {

enum {
  SPIRV_STARTING_WORD_INDEX = 5,
  SPIRV_WORD_SIZE           = sizeof(uint32_t),
  SPIRV_MINIMUM_FILE_SIZE   = SPIRV_STARTING_WORD_INDEX * SPIRV_WORD_SIZE
};

namespace util {

inline uint32_t RoundUp(uint32_t value, uint32_t multiple) 
{
    assert(multiple && ((multiple & (multiple -1)) == 0));
    return (value + multiple - 1) & ~(multiple - 1);
}

} // namespace util

// =================================================================================================
// Parser
// =================================================================================================
Parser::Parser()
{
}

Parser::Parser(const std::vector<uint32_t>& spirv_binary_0,
               const std::vector<uint32_t>& spirv_binary_1,
               const std::vector<uint32_t>& spirv_binary_2,
               const std::vector<uint32_t>& spirv_binary_3,
               const std::vector<uint32_t>& spirv_binary_4,
               const std::vector<uint32_t>& spirv_binary_5)
{
  std::vector<const std::vector<uint32_t>*> stream_ptrs;

  if (! spirv_binary_0.empty()) {
    stream_ptrs.push_back(&spirv_binary_0);
  }
  
  if (! spirv_binary_1.empty()) {
    stream_ptrs.push_back(&spirv_binary_1);
  }

  if (! spirv_binary_2.empty()) {
    stream_ptrs.push_back(&spirv_binary_2);
  }

  if (! spirv_binary_3.empty()) {
    stream_ptrs.push_back(&spirv_binary_3);
  }

  if (! spirv_binary_4.empty()) {
    stream_ptrs.push_back(&spirv_binary_4);
  }
  
  if (! spirv_binary_5.empty()) {
    stream_ptrs.push_back(&spirv_binary_5);
  }

  for (size_t i = 0; i < stream_ptrs.size(); ++i) {
    const auto& stream_ptr = stream_ptrs[i];
    ParseSpirv(*stream_ptr);
    if (! IsValid()) {
      std::stringstream ss;
      ss << "Failed parsing SPIR-V binary stream: " << i;
      m_msgs.push_back(ss.str());
      break;
    }
  }

  ParseFinalize();
}

Parser::Parser(const std::string& file_path_0, 
               const std::string& file_path_1, 
               const std::string& file_path_2, 
               const std::string& file_path_3, 
               const std::string& file_path_4, 
               const std::string& file_path_5)
{
  std::vector<std::string> file_paths;
  
  if (! file_path_0.empty()) {
    file_paths.push_back(file_path_0);
  }
  
  if (! file_path_1.empty()) {
    file_paths.push_back(file_path_1);
  }

  if (! file_path_2.empty()) {
    file_paths.push_back(file_path_2);
  }

  if (! file_path_3.empty()) {
    file_paths.push_back(file_path_3);
  }

  if (! file_path_4.empty()) {
    file_paths.push_back(file_path_4);
  }

  if (! file_path_5.empty()) {
    file_paths.push_back(file_path_5);
  }

  for (const auto& file_path : file_paths) {
    ParseSpirv(file_path);
    if (! IsValid()) {
      std::stringstream ss;
      ss << "Failed parsing SPIR_V file: " << file_path;
      m_msgs.push_back(ss.str());
      break;
    }
  }

  ParseFinalize();
}

Parser::~Parser()
{
}

void Parser::ParseFinalize()
{
  for (auto& spirv : m_spirv) {
    if (IsValid()) {
      ParseInputsAndOutputs(&spirv);
    }

    if (IsValid()) {
      ParseDescriptorSets(&spirv);
    }

    switch (spirv.execution_model) {
      case spv::ExecutionModelVertex                 : m_vert_spirv = &spirv; break;
      case spv::ExecutionModelTessellationControl    : m_tesc_spirv = &spirv; break;
      case spv::ExecutionModelTessellationEvaluation : m_tese_spirv = &spirv; break;
      case spv::ExecutionModelGeometry               : m_geom_spirv = &spirv; break;
      case spv::ExecutionModelFragment               : m_frag_spirv = &spirv; break;
      case spv::ExecutionModelGLCompute              : m_comp_spirv = &spirv; break;
    }
  }
}

std::vector<const DescriptorSetDesc*> Parser::GetDescriptorSets(spv::ExecutionModel stage) const
{
  auto it = std::find_if(std::begin(m_spirv),
                         std::end(m_spirv),
                         [stage](const Spirv& elem) -> bool {
                           return elem.execution_model == stage; });

  std::vector<const DescriptorSetDesc*> descs;
  if (it != std::end(m_spirv)) {
    for (const auto& desc : it->descriptor_set_descs) {
      descs.push_back(&desc);
    }
  }
  return descs;
}

void Parser::ParseSpirv(const std::string& file_path)
{
  std::ifstream is(file_path.c_str(), std::ios::binary);
  if (! is.is_open()) {
    m_error = ERROR_UNEXPECTED_EOF;
    return;
  }

  is.seekg(0, std::ios::end);
  size_t file_size = is.tellg();
  if ((file_size < SPIRV_MINIMUM_FILE_SIZE) || ((file_size % spirv_vars::SPIRV_WORD_SIZE) != 0)) {
    return;
  }
    
  is.seekg(0, std::ios::beg);  
  size_t word_count = file_size / spirv_vars::SPIRV_WORD_SIZE;
  std::vector<uint32_t> spirv_binary(word_count);
  is.read(reinterpret_cast<char*>(spirv_binary.data()), file_size);

  SpirvHeader header = {};
  std::memcpy(static_cast<void*>(&header), 
              static_cast<const void*>(spirv_binary.data()),
              sizeof(header));

  ParseSpirv(spirv_binary);
}

void Parser::ParseSpirv(const std::vector<uint32_t>& spirv_binary)
{
  size_t spirv_word_index = SPIRV_STARTING_WORD_INDEX;
  const size_t spirv_word_count = spirv_binary.size();

  if (spirv_word_index >= spirv_word_count) {
    m_error = spirv_vars::ERROR_UNEXPECTED_EOF;
    return;
  }

  Spirv spirv = {};
  m_cs = &spirv;

  // Parse instructions
  uint32_t opcode_index = 0;
  while (spirv_word_index < spirv_word_count) {
    uint32_t word = spirv_binary[spirv_word_index];
    uint32_t opcode_value = word & 0xFFFF;
    spv::Op instr_opcode = static_cast<spv::Op>(opcode_value);
    uint32_t instr_word_count = (word >> 16) & 0xFFFF;
    const uint32_t* instr_words = &(spirv_binary[spirv_word_index]);

    ParseInstruction(instr_opcode, instr_word_count, instr_words);
    
    if (m_error != ERROR_NONE) {
      break;
    }
   
    spirv_word_index += instr_word_count;
    opcode_index += 1;
  }

  spv::ExecutionModel execution_model = spirv.execution_model;
  auto it = std::find_if(std::begin(m_spirv),
                         std::end(m_spirv),
                         [execution_model](const Spirv& elem) -> bool {
                           return elem.execution_model == execution_model; });


  if (it == std::end(m_spirv)) {
    m_spirv.push_back(spirv);
  }
  else {
    m_error = ERROR_DUPLICATE_SHADER_STAGE;
  }

  m_cs = nullptr;
}

void Parser::ParseInstruction(spv::Op instr_opcode, 
                              uint32_t instr_word_count, 
                              const uint32_t* instr_words)
{
  m_ip = {};
  m_ip.instr_word_count = instr_word_count;
  m_ip.instr_words = instr_words;
  m_ip.instr_word_index = 1;

  switch (instr_opcode) {
    //case spv::OpNop: break; 
    //case spv::OpUndef: break; 
    //case spv::OpSourceContinued: break; 
    //case spv::OpSource: break; 
    //case spv::OpSourceExtension: break; 
    
    case spv::OpName: {
      uint32_t result_id = parse_operand_uint();
      std::string name = parse_operand_string();
      if (IsValid()) {
        m_cs->results[result_id].id = result_id;
        m_cs->results[result_id].name = name;
      }
    }
    break; 

    case spv::OpMemberName: {
      uint32_t result_id = parse_operand_uint();
      uint32_t member_index = parse_operand_uint();
      std::string name = parse_operand_string();
      if (IsValid()) {
        m_cs->results[result_id].id = result_id;
        m_cs->results[result_id].struct_members[member_index].index = member_index;
        m_cs->results[result_id].struct_members[member_index].name = name;
      }
    }
    break; 

    //case spv::OpString: break; 
    //case spv::OpLine: break; 
    //case spv::OpExtension: break; 
    //case spv::OpExtInstImport: break; 
    //case spv::OpExtInst: break; 
    //case spv::OpMemoryModel: break; 
    
    case spv::OpEntryPoint: {
      uint32_t execution_model = parse_operand_uint(); 
      uint32_t entry_point_id = parse_operand_uint();
      std::string name = parse_operand_string();
      std::vector<uint32_t> interfaces;
      while (has_next_operand()) {
        uint32_t intf = parse_operand_uint();
        interfaces.push_back(intf);
      }
      if (IsValid()) {
        m_cs->execution_model = static_cast<spv::ExecutionModel>(execution_model);
        m_cs->entry_point_id = entry_point_id;
        m_cs->name = name;
        m_cs->interfaces = interfaces;
      }
    }
    break;

    //case spv::OpExecutionMode: break; 
    //case spv::OpCapability: break; 
    
    case spv::OpTypeVoid: { 
      uint32_t result_id = parse_operand_uint(); 
      if (IsValid()) {
        m_cs->results[result_id].id = result_id;
        m_cs->results[result_id].op_type = instr_opcode;
      }
    } 
    break; 

    case spv::OpTypeBool: { 
      uint32_t result_id = parse_operand_uint(); 
      if (IsValid()) {
        m_cs->results[result_id].id = result_id;
        m_cs->results[result_id].op_type = instr_opcode;
      }
    } 
    break; 

    case spv::OpTypeInt: { 
      uint32_t result_id = parse_operand_uint(); 
      uint32_t width = parse_operand_uint();
      uint32_t signedness = parse_operand_uint();
      if (IsValid()) {
        m_cs->results[result_id].id = result_id;
        m_cs->results[result_id].op_type = instr_opcode;
        m_cs->results[result_id].traits_int.width = width;
        m_cs->results[result_id].traits_int.signedness = signedness;
      }
    } 
    break; 

    case spv::OpTypeFloat: { 
      uint32_t result_id = parse_operand_uint(); 
      uint32_t width = parse_operand_uint();
      if (IsValid()) {
        m_cs->results[result_id].id = result_id;
        m_cs->results[result_id].op_type = instr_opcode;
        m_cs->results[result_id].traits_float.width = width;
      }
    } 
    break; 

    case spv::OpTypeVector: { 
      uint32_t result_id = parse_operand_uint();
      uint32_t component_type = parse_operand_uint();      
      uint32_t component_count = parse_operand_uint();
      if (IsValid()) {
        m_cs->results[result_id].id = result_id;
        m_cs->results[result_id].op_type = instr_opcode;
        m_cs->results[result_id].traits_vector.component_type_id = component_type;
        m_cs->results[result_id].traits_vector.component_count = component_count;
      }
    } 
    break; 
    
    case spv::OpTypeMatrix: { 
      uint32_t result_id = parse_operand_uint();
      uint32_t column_type = parse_operand_uint();      
      uint32_t column_count = parse_operand_uint();
      if (IsValid()) {
        m_cs->results[result_id].id = result_id;
        m_cs->results[result_id].op_type = instr_opcode;
        m_cs->results[result_id].traits_matrix.column_type_id = column_type;
        m_cs->results[result_id].traits_matrix.column_count = column_count;
      }
    } 
    break; 

    case spv::OpTypeImage: { 
      uint32_t result_id = parse_operand_uint(); 
      uint32_t sampled_type_id = parse_operand_uint(); 
      uint32_t dim = parse_operand_uint(); 
      uint32_t depth = parse_operand_uint(); 
      uint32_t arrayed = parse_operand_uint(); 
      uint32_t multi_sample = parse_operand_uint(); 
      uint32_t sampled = parse_operand_uint(); 
      uint32_t image_format = parse_operand_uint();
      uint32_t access_qualifier = UINT32_MAX;
      if (has_next_operand()) {
        access_qualifier = parse_operand_uint(); 
      }
      if (IsValid()) {
        auto& result = m_cs->results[result_id];
        result.id = result_id;
        result.op_type = instr_opcode;
        result.traits_image.sampled_type_id  = sampled_type_id;
        result.traits_image.dim  = static_cast<spv::Dim>(dim);
        result.traits_image.depth  = depth;
        result.traits_image.arrayed  = arrayed;
        result.traits_image.multi_sample  = multi_sample;
        result.traits_image.sampled  = sampled;
        result.traits_image.image_format  = static_cast<spv::ImageFormat>(image_format);
        result.traits_image.access_qualifier  = access_qualifier;
      }
    } 
    break; 
    
    case spv::OpTypeSampler: { 
      uint32_t result_id = parse_operand_uint(); 
      if (IsValid()) {
        m_cs->results[result_id].id = result_id;
        m_cs->results[result_id].op_type = instr_opcode;
      }
    } 
    break; 

    case spv::OpTypeSampledImage: { 
      uint32_t result_id = parse_operand_uint(); 
      uint32_t image_type = parse_operand_uint(); 
      if (IsValid()) {
        m_cs->results[result_id].id = result_id;
        m_cs->results[result_id].op_type = instr_opcode;
        m_cs->results[result_id].traits_sampled_image.image_type = image_type;
      }
    } 
    break;
    
    case spv::OpTypeArray: { 
      uint32_t result_id = parse_operand_uint(); 
      uint32_t element_type = parse_operand_uint();       
      uint32_t length = parse_operand_uint(); 
      if (IsValid()) {
        m_cs->results[result_id].id = result_id;
        m_cs->results[result_id].op_type = instr_opcode;
        m_cs->results[result_id].traits_array.element_type_id = element_type;
        m_cs->results[result_id].traits_array.length_id = length;
      }
    } 
    break;

    case spv::OpTypeRuntimeArray: { 
      uint32_t result_id = parse_operand_uint(); 
      uint32_t element_type = parse_operand_uint();       
      if (IsValid()) {
        m_cs->results[result_id].id = result_id;
        m_cs->results[result_id].op_type = instr_opcode;
        m_cs->results[result_id].traits_runtime_array.element_type = element_type;
      }
    } 
    break; 
    
    case spv::OpTypeStruct: { 
      uint32_t result_id = parse_operand_uint();
      std::vector<uint32_t> member_type_ids;
      while (has_next_operand()) {
        uint32_t member_type_id = parse_operand_uint();
        member_type_ids.push_back(member_type_id);
      }
      if (IsValid()) {
        m_cs->results[result_id].id = result_id;
        m_cs->results[result_id].op_type = instr_opcode;
        for (uint32_t member_index = 0; member_index < member_type_ids.size(); ++member_index) {
          uint32_t member_type_id = member_type_ids[member_index];
          m_cs->results[result_id].struct_members[member_index].index = member_index;
          m_cs->results[result_id].struct_members[member_index].type_id = member_type_id;
        }
      }
    } 
    break;
    
    //case spv::OpTypeOpaque: break; 

    case spv::OpTypePointer: { 
      uint32_t result_id = parse_operand_uint();
      uint32_t storage_class = parse_operand_uint();
      uint32_t type_id = parse_operand_uint();
      if (IsValid()) {
        m_cs->results[result_id].id = result_id;
        m_cs->results[result_id].op_type = instr_opcode;
        m_cs->results[result_id].storage_class = static_cast<spv::StorageClass>(storage_class);
        m_cs->results[result_id].type_id = type_id;
      }
    } 
    break;

    //case spv::OpTypeFunction: break; 
    //case spv::OpTypeEvent: break; 
    //case spv::OpTypeDeviceEvent: break; 
    //case spv::OpTypeReserveId: break; 
    //case spv::OpTypeQueue: break; 
    //case spv::OpTypePipe: break; 
    //case spv::OpTypeForwardPointer: break; 
    //case spv::OpConstantTrue: break; 
    //case spv::OpConstantFalse: break; 
    
    case spv::OpConstant: {      
      uint32_t result_type_id = parse_operand_uint();
      uint32_t result_id = parse_operand_uint();
      std::vector<uint32_t> values;
      while (has_next_operand()) {
        uint32_t value = parse_operand_uint();
        values.push_back(value);
      }
      if (IsValid()) {
        m_cs->results[result_id].id = result_id;
        m_cs->results[result_id].op_type = instr_opcode;
        m_cs->results[result_id].type_id = result_type_id;
        m_cs->results[result_id].constant_values = values;
      }
    } 
    break;

    //case spv::OpConstantComposite: break; 
    //case spv::OpConstantSampler: break; 
    //case spv::OpConstantNull: break; 
    //case spv::OpSpecConstantTrue: break; 
    //case spv::OpSpecConstantFalse: break; 
    //case spv::OpSpecConstant: break; 
    //case spv::OpSpecConstantComposite: break; 
    //case spv::OpSpecConstantOp: break; 
    //case spv::OpFunction: break; 
    //case spv::OpFunctionParameter: break; 
    //case spv::OpFunctionEnd: break; 
    //case spv::OpFunctionCall: break; 
    
    case spv::OpVariable: { 
      uint32_t result_type_id = parse_operand_uint();
      uint32_t result_id = parse_operand_uint();
      uint32_t storage_class_value = parse_operand_uint();
      spv::StorageClass storage_class = static_cast<spv::StorageClass>(storage_class_value);
      uint32_t initializer_id = 0;
      if (has_next_operand()) {
        initializer_id = parse_operand_uint();
      }
      if (IsValid()) {
        m_cs->results[result_id].id = result_id;
        m_cs->results[result_id].op_type = instr_opcode;
        m_cs->results[result_id].type_id = result_type_id;
        m_cs->results[result_id].storage_class = storage_class;
      }
    } 
    break;

    //case spv::OpImageTexelPointer: break; 
    
    case spv::OpLoad: { 
      uint32_t result_type_id = parse_operand_uint();
      uint32_t result_id = parse_operand_uint();
      uint32_t pointer_id = parse_operand_uint();
      uint32_t memory_access = UINT32_MAX;
      if (has_next_operand()) {
        memory_access = parse_operand_uint();
      }
      if (IsValid()) {
        m_cs->results[result_id].id = result_id;
        m_cs->results[result_id].op_type = instr_opcode;
        m_cs->results[pointer_id].accessed = true;
      }
    } 
    break;

    case spv::OpStore: { 
      uint32_t pointer_id = parse_operand_uint();
      uint32_t object_id = parse_operand_uint();
      uint32_t memory_access = UINT32_MAX;
      if (has_next_operand()) {
        memory_access = parse_operand_uint();
      }
      if (IsValid()) {
        m_cs->results[pointer_id].accessed = true;
      }
    } 
    break;


    //case spv::OpCopyMemory: break; 
    //case spv::OpCopyMemorySized: break; 
    
    case spv::OpAccessChain: { 
      uint32_t result_type_id = parse_operand_uint();
      uint32_t result_id = parse_operand_uint();
      uint32_t base_id = parse_operand_uint();
      std::vector<uint32_t> ids;
      while (has_next_operand()) {
        uint32_t id = parse_operand_uint();
        ids.push_back(id);
      }
      if (IsValid()) {
        m_cs->results[result_id].id = result_id;
        m_cs->results[result_id].op_type = instr_opcode;
        m_cs->results[base_id].accessed = true;
      }
    } 
    break;

    //case spv::OpInBoundsAccessChain: break; 
    //case spv::OpPtrAccessChain: break; 
    //case spv::OpArrayLength: break; 
    //case spv::OpGenericPtrMemSemantics: break; 
    //case spv::OpInBoundsPtrAccessChain: break; 
    
    case spv::OpDecorate: {
      uint32_t target_id = parse_operand_uint();
      uint32_t member_index = UINT32_MAX;
      uint32_t decoration_value = parse_operand_uint();
      spv::Decoration decoration = static_cast<spv::Decoration>(decoration_value);
      std::vector<uint32_t> words;
      while (has_next_operand()) {
        uint32_t word = parse_operand_uint();
        words.push_back(word);
      }
      ParseDecoration(target_id, member_index, decoration, words);
    }
    break;

    case spv::OpMemberDecorate: {
      uint32_t target_id = parse_operand_uint();
      uint32_t member_index = parse_operand_uint();
      uint32_t decoration_value = parse_operand_uint();
      spv::Decoration decoration = static_cast<spv::Decoration>(decoration_value);
      std::vector<uint32_t> words;
      while (has_next_operand()) {
        uint32_t word = parse_operand_uint();
        words.push_back(word);
      }
      ParseDecoration(target_id, member_index, decoration, words);
    }
    break;

    //case spv::OpDecorationGroup: break; 
    //case spv::OpGroupDecorate: break; 
    //case spv::OpGroupMemberDecorate: break; 
    //case spv::OpVectorExtractDynamic: break; 
    //case spv::OpVectorInsertDynamic: break; 
    //case spv::OpVectorShuffle: break; 
    //case spv::OpCompositeConstruct: break; 
    //case spv::OpCompositeExtract: break; 
    //case spv::OpCompositeInsert: break; 
    //case spv::OpCopyObject: break; 
    //case spv::OpTranspose: break; 
    //case spv::OpSampledImage: break; 
    //case spv::OpImageSampleImplicitLod: break; 
    //case spv::OpImageSampleExplicitLod: break; 
    //case spv::OpImageSampleDrefImplicitLod: break; 
    //case spv::OpImageSampleDrefExplicitLod: break; 
    //case spv::OpImageSampleProjImplicitLod: break; 
    //case spv::OpImageSampleProjExplicitLod: break; 
    //case spv::OpImageSampleProjDrefImplicitLod: break; 
    //case spv::OpImageSampleProjDrefExplicitLod: break; 
    //case spv::OpImageFetch: break; 
    //case spv::OpImageGather: break; 
    //case spv::OpImageDrefGather: break; 
    //case spv::OpImageRead: break; 
    //case spv::OpImageWrite: break; 
    //case spv::OpImage: break; 
    //case spv::OpImageQueryFormat: break; 
    //case spv::OpImageQueryOrder: break; 
    //case spv::OpImageQuerySizeLod: break; 
    //case spv::OpImageQuerySize: break; 
    //case spv::OpImageQueryLod: break; 
    //case spv::OpImageQueryLevels: break; 
    //case spv::OpImageQuerySamples: break; 
    //case spv::OpConvertFToU: break; 
    //case spv::OpConvertFToS: break; 
    //case spv::OpConvertSToF: break; 
    //case spv::OpConvertUToF: break; 
    //case spv::OpUConvert: break; 
    //case spv::OpSConvert: break; 
    //case spv::OpFConvert: break; 
    //case spv::OpQuantizeToF16: break; 
    //case spv::OpConvertPtrToU: break; 
    //case spv::OpSatConvertSToU: break; 
    //case spv::OpSatConvertUToS: break; 
    //case spv::OpConvertUToPtr: break; 
    //case spv::OpPtrCastToGeneric: break; 
    //case spv::OpGenericCastToPtr: break; 
    //case spv::OpGenericCastToPtrExplicit: break; 
    //case spv::OpBitcast: break; 
    //case spv::OpSNegate: break; 
    //case spv::OpFNegate: break; 
    //case spv::OpIAdd: break; 
    //case spv::OpFAdd: break; 
    //case spv::OpISub: break; 
    //case spv::OpFSub: break; 
    //case spv::OpIMul: break; 
    //case spv::OpFMul: break; 
    //case spv::OpUDiv: break; 
    //case spv::OpSDiv: break; 
    //case spv::OpFDiv: break; 
    //case spv::OpUMod: break; 
    //case spv::OpSRem: break; 
    //case spv::OpSMod: break; 
    //case spv::OpFRem: break; 
    //case spv::OpFMod: break; 
    //case spv::OpVectorTimesScalar: break; 
    //case spv::OpMatrixTimesScalar: break; 
    //case spv::OpVectorTimesMatrix: break; 
    //case spv::OpMatrixTimesVector: break; 
    //case spv::OpMatrixTimesMatrix: break; 
    //case spv::OpOuterProduct: break; 
    //case spv::OpDot: break; 
    //case spv::OpIAddCarry: break; 
    //case spv::OpISubBorrow: break; 
    //case spv::OpUMulExtended: break; 
    //case spv::OpSMulExtended: break; 
    //case spv::OpAny: break; 
    //case spv::OpAll: break; 
    //case spv::OpIsNan: break; 
    //case spv::OpIsInf: break; 
    //case spv::OpIsFinite: break; 
    //case spv::OpIsNormal: break; 
    //case spv::OpSignBitSet: break; 
    //case spv::OpLessOrGreater: break; 
    //case spv::OpOrdered: break; 
    //case spv::OpUnordered: break; 
    //case spv::OpLogicalEqual: break; 
    //case spv::OpLogicalNotEqual: break; 
    //case spv::OpLogicalOr: break; 
    //case spv::OpLogicalAnd: break; 
    //case spv::OpLogicalNot: break; 
    //case spv::OpSelect: break; 
    //case spv::OpIEqual: break; 
    //case spv::OpINotEqual: break; 
    //case spv::OpUGreaterThan: break; 
    //case spv::OpSGreaterThan: break; 
    //case spv::OpUGreaterThanEqual: break; 
    //case spv::OpSGreaterThanEqual: break; 
    //case spv::OpULessThan: break; 
    //case spv::OpSLessThan: break; 
    //case spv::OpULessThanEqual: break; 
    //case spv::OpSLessThanEqual: break; 
    //case spv::OpFOrdEqual: break; 
    //case spv::OpFUnordEqual: break; 
    //case spv::OpFOrdNotEqual: break; 
    //case spv::OpFUnordNotEqual: break; 
    //case spv::OpFOrdLessThan: break; 
    //case spv::OpFUnordLessThan: break; 
    //case spv::OpFOrdGreaterThan: break; 
    //case spv::OpFUnordGreaterThan: break; 
    //case spv::OpFOrdLessThanEqual: break; 
    //case spv::OpFUnordLessThanEqual: break; 
    //case spv::OpFOrdGreaterThanEqual: break; 
    //case spv::OpFUnordGreaterThanEqual: break; 
    //case spv::OpShiftRightLogical: break; 
    //case spv::OpShiftRightArithmetic: break; 
    //case spv::OpShiftLeftLogical: break; 
    //case spv::OpBitwiseOr: break; 
    //case spv::OpBitwiseXor: break; 
    //case spv::OpBitwiseAnd: break; 
    //case spv::OpNot: break; 
    //case spv::OpBitFieldInsert: break; 
    //case spv::OpBitFieldSExtract: break; 
    //case spv::OpBitFieldUExtract: break; 
    //case spv::OpBitReverse: break; 
    //case spv::OpBitCount: break; 
    //case spv::OpDPdx: break; 
    //case spv::OpDPdy: break; 
    //case spv::OpFwidth: break; 
    //case spv::OpDPdxFine: break; 
    //case spv::OpDPdyFine: break; 
    //case spv::OpFwidthFine: break; 
    //case spv::OpDPdxCoarse: break; 
    //case spv::OpDPdyCoarse: break; 
    //case spv::OpFwidthCoarse: break; 
    //case spv::OpEmitVertex: break; 
    //case spv::OpEndPrimitive: break; 
    //case spv::OpEmitStreamVertex: break; 
    //case spv::OpEndStreamPrimitive: break; 
    //case spv::OpControlBarrier: break; 
    //case spv::OpMemoryBarrier: break; 
    //case spv::OpAtomicLoad: break; 
    //case spv::OpAtomicStore: break; 
    //case spv::OpAtomicExchange: break; 
    //case spv::OpAtomicCompareExchange: break; 
    //case spv::OpAtomicCompareExchangeWeak: break; 
    //case spv::OpAtomicIIncrement: break; 
    //case spv::OpAtomicIDecrement: break; 
    //case spv::OpAtomicIAdd: break; 
    //case spv::OpAtomicISub: break; 
    //case spv::OpAtomicSMin: break; 
    //case spv::OpAtomicUMin: break; 
    //case spv::OpAtomicSMax: break; 
    //case spv::OpAtomicUMax: break; 
    //case spv::OpAtomicAnd: break; 
    //case spv::OpAtomicOr: break; 
    //case spv::OpAtomicXor: break; 
    //case spv::OpPhi: break; 
    //case spv::OpLoopMerge: break; 
    //case spv::OpSelectionMerge: break; 
    //case spv::OpLabel: break; 
    //case spv::OpBranch: break; 
    //case spv::OpBranchConditional: break; 
    //case spv::OpSwitch: break; 
    //case spv::OpKill: break; 
    //case spv::OpReturn: break; 
    //case spv::OpReturnValue: break; 
    //case spv::OpUnreachable: break; 
    //case spv::OpLifetimeStart: break; 
    //case spv::OpLifetimeStop: break; 
    //case spv::OpGroupAsyncCopy: break; 
    //case spv::OpGroupWaitEvents: break; 
    //case spv::OpGroupAll: break; 
    //case spv::OpGroupAny: break; 
    //case spv::OpGroupBroadcast: break; 
    //case spv::OpGroupIAdd: break; 
    //case spv::OpGroupFAdd: break; 
    //case spv::OpGroupFMin: break; 
    //case spv::OpGroupUMin: break; 
    //case spv::OpGroupSMin: break; 
    //case spv::OpGroupFMax: break; 
    //case spv::OpGroupUMax: break; 
    //case spv::OpGroupSMax: break; 
    //case spv::OpReadPipe: break; 
    //case spv::OpWritePipe: break; 
    //case spv::OpReservedReadPipe: break; 
    //case spv::OpReservedWritePipe: break; 
    //case spv::OpReserveReadPipePackets: break; 
    //case spv::OpReserveWritePipePackets: break; 
    //case spv::OpCommitReadPipe: break; 
    //case spv::OpCommitWritePipe: break; 
    //case spv::OpIsValidReserveId: break; 
    //case spv::OpGetNumPipePackets: break; 
    //case spv::OpGetMaxPipePackets: break; 
    //case spv::OpGroupReserveReadPipePackets: break; 
    //case spv::OpGroupReserveWritePipePackets: break; 
    //case spv::OpGroupCommitReadPipe: break; 
    //case spv::OpGroupCommitWritePipe: break; 
    //case spv::OpEnqueueMarker: break; 
    //case spv::OpEnqueueKernel: break; 
    //case spv::OpGetKernelNDrangeSubGroupCount: break; 
    //case spv::OpGetKernelNDrangeMaxSubGroupSize: break; 
    //case spv::OpGetKernelWorkGroupSize: break; 
    //case spv::OpGetKernelPreferredWorkGroupSizeMultiple: break; 
    //case spv::OpRetainEvent: break; 
    //case spv::OpReleaseEvent: break; 
    //case spv::OpCreateUserEvent: break; 
    //case spv::OpIsValidEvent: break; 
    //case spv::OpSetUserEventStatus: break; 
    //case spv::OpCaptureEventProfilingInfo: break; 
    //case spv::OpGetDefaultQueue: break; 
    //case spv::OpBuildNDRange: break; 
    //case spv::OpImageSparseSampleImplicitLod: break; 
    //case spv::OpImageSparseSampleExplicitLod: break; 
    //case spv::OpImageSparseSampleDrefImplicitLod: break; 
    //case spv::OpImageSparseSampleDrefExplicitLod: break; 
    //case spv::OpImageSparseSampleProjImplicitLod: break; 
    //case spv::OpImageSparseSampleProjExplicitLod: break; 
    //case spv::OpImageSparseSampleProjDrefImplicitLod: break; 
    //case spv::OpImageSparseSampleProjDrefExplicitLod: break; 
    //case spv::OpImageSparseFetch: break; 
    //case spv::OpImageSparseGather: break; 
    //case spv::OpImageSparseDrefGather: break; 
    //case spv::OpImageSparseTexelsResident: break; 
    //case spv::OpNoLine: break; 
    //case spv::OpAtomicFlagTestAndSet: break; 
    //case spv::OpAtomicFlagClear: break; 
    //case spv::OpImageSparseRead: break; 
    //case spv::OpSizeOf: break; 
    //case spv::OpTypePipeStorage: break; 
    //case spv::OpConstantPipeStorage: break; 
    //case spv::OpCreatePipeFromPipeStorage: break; 
    //case spv::OpGetKernelLocalSizeForSubgroupCount: break; 
    //case spv::OpGetKernelMaxNumSubgroups: break; 
    //case spv::OpTypeNamedBarrier: break; 
    //case spv::OpNamedBarrierInitialize: break; 
    //case spv::OpMemoryNamedBarrier: break; 
    //case spv::OpModuleProcessed: break; 
    //case spv::OpSubgroupBallotKHR: break; 
    //case spv::OpSubgroupFirstInvocationKHR: break; 
    //case spv::OpSubgroupAllKHR: break; 
    //case spv::OpSubgroupAnyKHR: break; 
    //case spv::OpSubgroupAllEqualKHR: break; 
    //case spv::OpSubgroupReadInvocationKHR: break;
    default: {
      m_unhandled_opcodes.insert(instr_opcode);
    }
    break;
  }

  m_ip = {};
}

void Parser::ParseDecoration(uint32_t target_id, 
                             uint32_t member_index, 
                             spv::Decoration decoration, 
                             const std::vector<uint32_t>& words)
{
  switch (decoration) {
    //case spv::DecorationRelaxedPrecision: break; 
    //case spv::DecorationSpecId: break; 
    
    case spv::DecorationBlock: {
      bool* target = (member_index != UINT32_MAX) 
                      ? nullptr
                      : &(m_cs->results[target_id].block);
      *target = true;
    }
    break;

    case spv::DecorationBufferBlock: {
      bool* target = (member_index != UINT32_MAX) 
                      ? nullptr
                      : &(m_cs->results[target_id].buffer_block);
      assert(target != nullptr);
      *target = true;
    }
    break;


    //case spv::DecorationRowMajor: break; 
    //case spv::DecorationColMajor: break; 
    //case spv::DecorationArrayStride: break; 
    //case spv::DecorationMatrixStride: break; 
    //case spv::DecorationGLSLShared: break; 
    //case spv::DecorationGLSLPacked: break; 
    //case spv::DecorationCPacked: break; 
    //case spv::DecorationBuiltIn: break; 
    //case spv::DecorationNoPerspective: break; 
    //case spv::DecorationFlat: break; 
    //case spv::DecorationPatch: break; 
    //case spv::DecorationCentroid: break; 
    //case spv::DecorationSample: break; 
    //case spv::DecorationInvariant: break; 
    //case spv::DecorationRestrict: break; 
    //case spv::DecorationAliased: break; 
    //case spv::DecorationVolatile: break; 
    //case spv::DecorationConstant: break; 
    //case spv::DecorationCoherent: break; 
    //case spv::DecorationNonWritable: break; 
    //case spv::DecorationNonReadable: break; 
    //case spv::DecorationUniform: break; 
    //case spv::DecorationSaturatedConversion: break; 
    //case spv::DecorationStream: break; 
    
    case spv::DecorationLocation: {
      if (words.size() == 1) {
        uint32_t location = words[0];
        uint32_t* target = (member_index != UINT32_MAX) 
                           ? &(m_cs->results[target_id].struct_members[member_index].location)
                           : &(m_cs->results[target_id].location);
        *target = location;
      }
      else {
        m_error = ERROR_INCOMPLETE_DECORATION;
      }
    }
    break;

    //case spv::DecorationComponent: break; 
    //case spv::DecorationIndex: break; 

    case spv::DecorationBinding: {
      if (words.size() == 1) {
        uint32_t binding = words[0];
        uint32_t* target = (member_index != UINT32_MAX) 
                           ? &(m_cs->results[target_id].struct_members[member_index].binding)
                           : &(m_cs->results[target_id].binding);
        *target = binding;
      }
      else {
        m_error = ERROR_INCOMPLETE_DECORATION;
      }
    }
    break;

    case spv::DecorationDescriptorSet: {
      if (words.size() == 1) {
        uint32_t descriptor_set = words[0];
        uint32_t* target = (member_index != UINT32_MAX) 
                           ? nullptr
                           : &(m_cs->results[target_id].descriptor_set);
        assert(target != nullptr);
        *target = descriptor_set;
      }
      else {
        m_error = ERROR_INCOMPLETE_DECORATION;
      }
    }
    break;

    case spv::DecorationOffset:  {
      if (words.size() == 1) {
        uint32_t offset = words[0];
        uint32_t* target = (member_index != UINT32_MAX) 
                           ? &(m_cs->results[target_id].struct_members[member_index].offset)
                           : &(m_cs->results[target_id].offset);
        *target = offset;
      }
      else {
        m_error = ERROR_INCOMPLETE_DECORATION;
      }
    }
    break;
      
    //case spv::DecorationXfbBuffer: break; 
    //case spv::DecorationXfbStride: break; 
    //case spv::DecorationFuncParamAttr: break; 
    //case spv::DecorationFPRoundingMode: break; 
    //case spv::DecorationFPFastMathMode: break; 
    //case spv::DecorationLinkageAttributes: break; 
    //case spv::DecorationNoContraction: break; 
    //case spv::DecorationInputAttachmentIndex: break; 
    //case spv::DecorationAlignment: break; 
    //case spv::DecorationMaxByteOffset: break; 
    //case spv::DecorationOverrideCoverageNV: break; 
    //case spv::DecorationPassthroughNV: break; 
    //case spv::DecorationViewportRelativeNV: break; 
    //case spv::DecorationSecondaryViewportRelativeNV: break; 
    default: {
      m_unhandled_decorations.insert(decoration);
    }
  }
}

void Parser::ParseInputsAndOutputs(Spirv* spirv)
{
  for (const auto& it : spirv->results) {
    const Result* result = &(it.second);

    // Skip everything that isn't in the interface list
    auto it = std::find_if(std::begin(spirv->interfaces),
                           std::end(spirv->interfaces),
                           [&result](uint32_t value) -> bool { return value == result->id; });
    if (it == std::end(spirv->interfaces)) {
      continue;;
    }

    switch (result->storage_class) {
      case spv::StorageClassInput: {
        spirv->inputs.push_back(result);
      }
      break;

      case spv::StorageClassOutput: {
        spirv->outputs.push_back(result);
      }
      break;
    }
  }
}

void Parser::ParseDescriptorSets(Spirv* spirv)
{
  const auto& results = spirv->results;

  for (const auto& it_result : results) {
    const Result* result = &(it_result.second);

    // Skip everything that doesn't have a sane value for a descriptor set or binding
    if ((result->descriptor_set == UINT32_MAX) || (result->binding == UINT32_MAX)) {
      continue;
    }

    // Skip everything that isn't accessed
    if (! result->accessed) {
      continue;
    }

    const Result* resolved = ResolveType(result, spirv);
    if (resolved == nullptr) {
      m_error = ERROR_TYPE_RESOLVE_FAILED;
      break;
    }

    // Descriptor set description
    DescriptorSetDesc* descriptor_set_desc = nullptr;
    {
      uint32_t descriptor_set = result->descriptor_set;
      auto it_desc = std::find_if(std::begin(spirv->descriptor_set_descs),
                                  std::end(spirv->descriptor_set_descs),
                                  [descriptor_set](const DescriptorSetDesc& elem) -> bool {
                                    return elem.set == descriptor_set; });
      if (it_desc != std::end(spirv->descriptor_set_descs)) {
        descriptor_set_desc = &(*it_desc);
      }
      else {
        DescriptorSetDesc desc = {};
        desc.set = descriptor_set;
        spirv->descriptor_set_descs.push_back(desc);
        descriptor_set_desc = &(spirv->descriptor_set_descs.back());
      }
    }

    // Descriptor binding description
    DescriptorBindingDesc* binding_desc = nullptr;
    {
      uint32_t binding = result->binding;
      auto it_desc = std::find_if(std::begin(descriptor_set_desc->binding_descs),
                                  std::end(descriptor_set_desc->binding_descs),
                                  [binding](const DescriptorBindingDesc& elem) -> bool {
                                    return elem.binding == binding; });
      if (it_desc != std::end(descriptor_set_desc->binding_descs)) {
        binding_desc = &(*it_desc);
      }
      else {
        DescriptorBindingDesc desc = {};
        desc.name = result->name;
        desc.binding = binding;
        desc.descriptorCount = 1;
        descriptor_set_desc->binding_descs.push_back(desc);
        binding_desc = &( descriptor_set_desc->binding_descs.back());
      }      
    }

    spv::Op resolved_op_type = resolved->op_type;

    // Arrays require extra resolving
    if (resolved_op_type == spv::OpTypeArray) {
      // Type
      auto it_element_type = spirv->results.find(resolved->traits_array.element_type_id);
      if (it_element_type != spirv->results.end()) {
        auto& element_result = it_element_type->second;
        resolved_op_type = element_result.op_type;
      }
      else {
        resolved_op_type = spv::OpNop;
      }
      // Count
      if (resolved_op_type != spv::OpNop) {
        auto it_length_type = spirv->results.find(resolved->traits_array.length_id);
        if (it_length_type != spirv->results.end()) {
          auto& length_result = it_length_type->second;
          assert(length_result.constant_values.size() > 0);
          // TODO: add check to make sure that resolved type for length is int
          binding_desc->descriptorCount = length_result.constant_values[0];
        }
      }
    }
   
    switch (resolved_op_type) {
      case spv::OpTypeImage: {
        binding_desc->descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
      }
      break;

      case spv::OpTypeSampler: {
        binding_desc->descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
      }
      break;

      case spv::OpTypeSampledImage: {
        binding_desc->descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      }
      break;

      case spv::OpTypeStruct: {
        // SSBO
        if (resolved->buffer_block) {
          binding_desc->descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        }
        // non-SSBO
        else if (resolved->block) {
          binding_desc->descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        }
        else {
          assert(false && "Unknown struct binding");
        }
      }
      break;
    }
  }

  // Sort descriptor sets
  std::sort(std::begin(spirv->descriptor_set_descs),
            std::end(spirv->descriptor_set_descs),
            [](const DescriptorSetDesc& a, const DescriptorSetDesc& b) -> bool {
               return a.set < b.set; });

  // Sort descriptor bindings
  for (auto& desc : spirv->descriptor_set_descs) {
    std::sort(std::begin(desc.binding_descs),
              std::end(desc.binding_descs),
              [](const DescriptorBindingDesc& a, const DescriptorBindingDesc& b) -> bool {
                 return a.binding < b.binding; });
  }
}


const Result* Parser::ResolveType(const Result* result, const Spirv* spirv) const
{
  const Result* resolved = result;
  while (resolved->type_id != 0) {
    uint32_t type_id = resolved->type_id;
    auto it = spirv->results.find(type_id);
    if (it != spirv->results.end()) {
      resolved = &(it->second);
    }
    else {
      resolved = nullptr;
    }
  }
  return resolved;
}

bool Parser::has_next_operand() const
{
  return m_ip.instr_word_index < m_ip.instr_word_count;
}

int32_t Parser::parse_operand_int()
{
  if (! has_next_operand()) {
    m_error = ERROR_INCOMPLETE_INSTRUCTION;
    return INT32_MAX;
  }

  uint32_t word = m_ip.instr_words[m_ip.instr_word_index];
  int32_t value = static_cast<int32_t>(word);

  uint32_t inc = 1;
  m_ip.instr_word_index += inc;

  return value;
}

uint32_t Parser::parse_operand_uint()
{
  if (! has_next_operand()) {
    m_error = ERROR_INCOMPLETE_INSTRUCTION;
    return UINT32_MAX;
  }

  uint32_t word = m_ip.instr_words[m_ip.instr_word_index];
  uint32_t value = static_cast<uint32_t>(word);

  uint32_t inc = 1;
  m_ip.instr_word_index += inc;

  return value;
}

std::string Parser::parse_operand_string()
{
  if (! has_next_operand()) {
    return std::string();
  }

  std::string value = reinterpret_cast<const char*>(&(m_ip.instr_words[m_ip.instr_word_index]));
  size_t length = value.length();

  uint32_t inc = util::RoundUp(length, SPIRV_WORD_SIZE) / SPIRV_WORD_SIZE;
  m_ip.instr_word_index += inc;

  return value;
}

} // namespace spirv_vars
#endif // SPIRV_VARS_IMPLEMENTATION

#endif // __SPIRV_VARS_HPP_INCLUDED__