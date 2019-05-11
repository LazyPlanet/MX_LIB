#pragma once

#include <google/protobuf/message.h>
#include <google/protobuf/compiler/importer.h>
#include <google/protobuf/compiler/parser.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/dynamic_message.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/tokenizer.h>

using namespace google::protobuf;

class Parser
{
public:
	Parser() { }

	static Parser& Instance()
	{
		static Parser _instance;
		return _instance;
	}

public:
	bool GenerateDescriptorPool(const std::string filePath);

	FileDescriptor* GetFileDescriptor() { return _file_descriptor; }
	DescriptorPool& GetDescriptorPool() { return _descriptor_pool; }
	DynamicMessageFactory& GetDynamicMessageFactory() { return _dynamic_message_factory; }
private:
	FileDescriptor* _file_descriptor;
	DynamicMessageFactory _dynamic_message_factory;
	DescriptorPool _descriptor_pool;
	bool _loader = false;
};