#include "stdafx.h"
#include "Reader.h"
#include "Parser.h"
#include "AnyLog.h"

#include <fstream>

namespace Asset {
	
#define MAX_FILE_SIZE 100000	

AssetManager::AssetManager() : __parse_sucess(false)
{

}
	
AssetManager::~AssetManager()
{
	 if (_thread) 
	 {
		 _thread->join();
		 delete _thread;
	 }
}

#undef GetMessage
	
void AssetManager::SetPath(const std::string& path)
{
	__asset_path = path; //��Դ·��
}

bool AssetManager::Load(const std::string& file_path)
{
	log_info("%s: start sync load", __func__);
	
	if (__parse_sucess) return true;

	__asset_path = file_path; //��Դ·��

	return OnLoad();
}

bool AssetManager::AsyncLoad(const std::string& file_path)
{
	log_info("%s: start async load", __func__);

	if (__parse_sucess) return true;

	__asset_path = file_path; //��Դ·��

	_thread = new std::thread(&AssetManager::OnLoad, this);

	return true;
}

bool AssetManager::OnLoad()
{
	log_info("%s: start load...", __func__);

	if (__parse_sucess) return true;

	__file_descriptor = Parser::Instance().GetFileDescriptor();
	if (!__file_descriptor) return false;

	const pb::EnumDescriptor* asset_type = __file_descriptor->FindEnumTypeByName("ASSET_TYPE");
	if (!asset_type) return false;

	//����������Դ�ṹ
	for (int i = 0; i < __file_descriptor->message_type_count(); ++i)
	{
		const pb::Descriptor* descriptor = __file_descriptor->message_type(i);
		if (!descriptor) return false;

		const pb::FieldDescriptor* field = descriptor->FindFieldByNumber(1);	//����MESSAGE�ĵ�һ����������������
		if (!field || field->enum_type() != asset_type) continue;

		const pb::Message* msg = Parser::Instance().GetDynamicMessageFactory().GetPrototype(descriptor);
		if (!msg) continue;

		const auto type_t = field->default_value_enum()->number();
		pb::Message* message = msg->New();
		if (__messages.find(type_t) == __messages.end())
		{
			__messages.insert(std::make_pair(type_t, message));	//�Ϸ�Э�飬����Ѿ���������ԣ���ֻ���ص�һ��Э��
		}
		else
		{
			return false;
		}
	}

	//����������Դ����
	fs::path full_path(__asset_path);
	if (!LoadAssets(full_path)) 
	{
		log_info("LoadAssets error.");
		return false;
	}

	log_info("%s:Load success and finished.", __func__);

	__parse_sucess = true;
	return true;
}

bool AssetManager::LoadAssets(fs::path& full_path)
{
	if (fs::exists(full_path))
	{
		fs::directory_iterator item_begin(full_path);
		fs::directory_iterator item_end;

		for (; item_begin != item_end; item_begin++)	
		{
			if (fs::is_directory(*item_begin))
			{
				std::string sub_dir_str(item_begin->path().string());
				fs::path sub_dir(sub_dir_str);
				LoadAssets(sub_dir);
			}
			else
			{
				if (item_begin->path().has_extension()) continue;

				const std::string filename = item_begin->path().string();
				//���ļ�
				std::fstream file(filename.c_str(), std::ios::in | std::ios::binary);
				if (!file) 
				{
					log_info("%s open file error, filename:%s", __func__, filename.c_str());
					return false; 	//���һ����������˳�
				}

				int32_t size = 0;
				file >> size;
				if (size == 0 || size > MAX_FILE_SIZE) 
				{
					log_info("%s open file error, size:%d", __func__, size);
					file.close();
					return false;	//�����ϵ����ļ����ᳬ��MAX_FILE_SIZE�ֽ�
				}

				char content[MAX_FILE_SIZE];
				file.read(content, size);

				//�ر��ļ�
				file.close();

				const std::string directory_string = item_begin->path().parent_path().string();
				if (directory_string == "") 
				{
					log_info("%s : directory_string %s is null.", __func__, directory_string.c_str());
					return false;
				}

				#if defined(WIN32)
	                int32_t found_pos = directory_string.find_last_of("\\");
				#else
	                int32_t found_pos = directory_string.find_last_of("//");
				#endif
				const std::string message_name = directory_string.substr(found_pos + 1);	//MESSAGE���Ƽ�Ϊ�ļ�������
				const pb::Descriptor* descriptor = this->__file_descriptor->FindMessageTypeByName(message_name);
				if (!descriptor) 
				{
					log_info("%s : descriptor is null.", __func__);
					return false;
				}

				const pb::Message* msg = Parser::Instance().GetDynamicMessageFactory().GetPrototype(descriptor);
				if (!msg)
				{
					log_info("%s : msg is null.", __func__);
					return false;
				}

				pb::Message* message = msg->New();
				const bool result = message->ParseFromArray(content, size);
				if (!result) return false;

				const pb::FieldDescriptor* type_field = message->GetDescriptor()->FindFieldByName("type_t");
				if (!type_field) return false; //���һ����������˳�

				int64_t global_id = 0;
				const pb::FieldDescriptor* prop_field = message->GetDescriptor()->FindFieldByName("common_prop");
				if (prop_field) //��ͨ��Դ
				{
					const pb::Message& prop_message = message->GetReflection()->GetMessage(*message, prop_field);
					const pb::FieldDescriptor* global_id_field = prop_message.GetDescriptor()->FindFieldByName("global_id");
					if (!global_id_field) return false;
					global_id = prop_message.GetReflection()->GetInt64(prop_message, global_id_field);
				}
				else //��Ʒ��Դ
				{
					const pb::FieldDescriptor* item_prop_field = message->GetDescriptor()->FindFieldByName("item_common_prop");
					if (!item_prop_field) return false;

					const pb::Message& item_prop_message = message->GetReflection()->GetMessage(*message, item_prop_field);
					prop_field = item_prop_message.GetDescriptor()->FindFieldByName("common_prop");
					if (!prop_field) return false;

					const pb::Message& prop_message = item_prop_message.GetReflection()->GetMessage(item_prop_message, prop_field);
					const pb::FieldDescriptor* global_id_field = prop_message.GetDescriptor()->FindFieldByName("global_id");
					if (!global_id_field) return false;
					global_id = prop_message.GetReflection()->GetInt64(prop_message, global_id_field);
				}
				//���ص�ȫ��Ψһ��
				_assets.insert(std::make_pair(global_id, message));
				//���ص����ͱ�
				const auto type_t = type_field->default_value_enum()->number();
				const auto type_name = type_field->default_value_enum()->name();
				__assets_bytypes[type_name].insert(message);
				__assets_name[global_id] = type_name;
				__bin_assets[global_id] = message->SerializeAsString();
			}
		}
	}

	return true;
}

pb::Message* AssetManager::GetMessage(int32_t message_type)
{
	const auto it = __messages.find(message_type);
	if (it == __messages.end()) return nullptr;
	return it->second;
}

std::set<pb::Message*> AssetManager::GetMessagesByType(std::string message_type)
{
	const auto it = __assets_bytypes.find(message_type);
	if (it == __assets_bytypes.end()) return {};
	return it->second;
}

pb::Message* AssetManager::Get(int64_t global_id)
{
	const auto it = _assets.find(global_id);
	if (it == _assets.end()) return nullptr;
	return it->second;
}

std::string AssetManager::GetTypeName(int64_t global_id)
{
	const auto it = __assets_name.find(global_id);
	if (it == __assets_name.end()) return "";
	return it->second;
}

std::string AssetManager::GetBinContent(int64_t global_id)
{
	const auto it = __bin_assets.find(global_id);
	if (it == __bin_assets.end()) return "";
	return it->second;
}

}

