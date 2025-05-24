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
	/// リソースを読み込む手段を提供するインターフェースクラス
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


