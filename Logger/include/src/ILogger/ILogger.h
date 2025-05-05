#pragma once
#include <cinttypes>

namespace utl
{
	enum class LogType : uint8_t
	{
		Debug,
		Warn,
		Error
	};

	class ILogger
	{
	protected:
		virtual ~ILogger() = default;

		virtual void Log(LogType level, const char* message, const char* file, int line) = 0;
	};

	class LoggerHolder
	{
	public:

		LoggerHolder(ILogger* logger);

		void Log(LogType level, const char* message, const char* file, int line);

		ILogger* Get()const
		{
			return logger_;
		}

	private:

		ILogger* logger_;

	};
}

// �f�o�b�O���O���c���}�N��
// �v���W�F�N�g�ݒ�̃v���v���Z�b�T��"UNENABLE_DEBUG"��ǉ������疳�����ł���
#if defined(UNENABLE_DEBUG)
#define LOG_DEBUG(logger, message)
#else
#define LOG_DEBUG(logger, message)\
logger.Log(utl::LogType::Debug, message, __FILE__, __LINE__);
#endif

// �x���̃��O���c���}�N��
// �v���W�F�N�g�ݒ�̃v���v���Z�b�T��"UNENABLE_WARNING"��ǉ������疳�����ł���
#if defined(UNENABLE_WARNING)
#define LOG_WARN(is,logger, message)
#else
#define LOG_WARN(is,logger, message)\
if(is)logger.Log(utl::LogType::Warn, message, __FILE__, __LINE__);
#endif

// �G���[�̃��O���c���}�N��
// �v���W�F�N�g�ݒ�̃v���v���Z�b�T��"UNENABLE_ERROR"��ǉ������疳�����ł���
#if defined(UNENABLE_ERROR)
#define LOG_ERROR(is,logger, message)
#else
#define LOG_ERROR(is,logger, message)\
if(is)logger.Log(utl::LogType::Error, message, __FILE__, __LINE__);
#endif
