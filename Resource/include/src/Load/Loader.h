#pragma once

namespace res
{
	class IResourceLoaderVoid
	{
	public:


		virtual ~IResourceLoaderVoid() = default;


		virtual bool Load(void* ptr) = 0;
	};

	/// <summary>
	/// ���\�[�X��ǂݍ��ގ�i��񋟂���C���^�[�t�F�[�X�N���X
	/// </summary>
	/// <typeparam name="ResType"></typeparam>
	template<typename ResType>
	class IResourceLoader :public IResourceLoaderVoid
	{
	public:


		virtual ~IResourceLoader() = default;


		//virtual bool Load(void* ptr)override = 0;

	};
}


