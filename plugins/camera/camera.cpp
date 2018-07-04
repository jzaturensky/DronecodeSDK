#include "camera.h"
#include "camera_impl.h"

namespace dronecode_sdk {

Camera::Camera(System &system) : PluginBase(), _impl{new CameraImpl(system)} {}

Camera::~Camera() {}

Camera::Result Camera::take_photo()
{
    return _impl->take_photo();
}

Camera::Result Camera::start_photo_interval(float interval_s)
{
    return _impl->start_photo_interval(interval_s);
}

Camera::Result Camera::stop_photo_interval()
{
    return _impl->stop_photo_interval();
}

Camera::Result Camera::start_video()
{
    return _impl->start_video();
}

Camera::Result Camera::stop_video()
{
    return _impl->stop_video();
}

void Camera::take_photo_async(const result_callback_t &callback)
{
    _impl->take_photo_async(callback);
}

void Camera::start_photo_interval_async(float interval_s, const result_callback_t &callback)
{
    _impl->start_photo_interval_async(interval_s, callback);
}

void Camera::stop_photo_interval_async(const result_callback_t &callback)
{
    _impl->stop_photo_interval_async(callback);
}

void Camera::start_video_async(const result_callback_t &callback)
{
    _impl->start_video_async(callback);
}

Camera::Result Camera::start_video_streaming()
{
    return _impl->start_video_streaming();
}

Camera::Result Camera::stop_video_streaming()
{
    return _impl->stop_video_streaming();
}

void Camera::set_video_stream_settings(const VideoStreamSettings &settings)
{
    return _impl->set_video_stream_settings(settings);
}

Camera::Result Camera::get_video_stream_info(VideoStreamInfo &info)
{
    return _impl->get_video_stream_info(info);
}

void Camera::get_video_stream_info_async(const get_video_stream_info_callback_t callback)
{
    _impl->get_video_stream_info_async(callback);
}

void Camera::subscribe_video_stream_info(const subscribe_video_stream_info_callback_t callback)
{
    _impl->subscribe_video_stream_info(callback);
}

void Camera::stop_video_async(const result_callback_t &callback)
{
    _impl->stop_video_async(callback);
}

Camera::Result Camera::set_mode(const Mode mode)
{
    return _impl->set_mode(mode);
}

void Camera::set_mode_async(const Mode mode, const mode_callback_t &callback)
{
    _impl->set_mode_async(mode, callback);
}

void Camera::get_mode_async(const mode_callback_t &callback)
{
    _impl->get_mode_async(callback);
}

void Camera::subscribe_mode(const subscribe_mode_callback_t callback)
{
    _impl->subscribe_mode(callback);
}

void Camera::get_status_async(get_status_callback_t callback)
{
    _impl->get_status_async(callback);
}

void Camera::subscribe_status(const Camera::subscribe_status_callback_t callback)
{
    _impl->subscribe_status(callback);
}

void Camera::subscribe_capture_info(capture_info_callback_t callback)
{
    _impl->subscribe_capture_info(callback);
}

void Camera::set_option_async(const std::string &setting_id,
                              const Option &option,
                              const result_callback_t &callback)
{
    _impl->set_option_async(setting_id, option, callback);
}

Camera::Result Camera::get_option(const std::string &setting_id, Option &option)
{
    return _impl->get_option(setting_id, option);
}

void Camera::get_option_async(const std::string &setting_id, const get_option_callback_t &callback)
{
    _impl->get_option_async(setting_id, callback);
}

bool Camera::get_possible_setting_options(std::vector<std::string> &settings)
{
    return _impl->get_possible_setting_options(settings);
}

bool Camera::get_possible_options(const std::string &setting_id,
                                  std::vector<Camera::Option> &options)
{
    return _impl->get_possible_options(setting_id, options);
}

bool Camera::get_setting_str(const std::string &setting_id, std::string &description)
{
    return _impl->get_setting_str(setting_id, description);
}

bool Camera::get_option_str(const std::string &setting_id,
                            const std::string &option_id,
                            std::string &description)
{
    return _impl->get_option_str(setting_id, option_id, description);
}

void Camera::subscribe_current_settings(const subscribe_current_settings_callback_t &callback)
{
    _impl->subscribe_current_settings(callback);
}

void Camera::subscribe_possible_setting_options(
    const subscribe_possible_setting_options_callback_t &callback)
{
    _impl->subscribe_possible_setting_options(callback);
}

std::string Camera::result_str(Result result)
{
    switch (result) {
        case Result::SUCCESS:
            return "Success";
        case Result::IN_PROGRESS:
            return "In progress";
        case Result::BUSY:
            return "Busy";
        case Result::DENIED:
            return "Denied";
        case Result::ERROR:
            return "Error";
        case Result::TIMEOUT:
            return "Timeout";
        case Result::WRONG_ARGUMENT:
            return "Wrong argument";
        case Result::UNKNOWN:
        // FALLTHROUGH
        default:
            return "Unknown";
    }
}

bool operator==(const Camera::VideoStreamSettings &lhs, const Camera::VideoStreamSettings &rhs)
{
    return lhs.frame_rate_hz == rhs.frame_rate_hz &&
           lhs.horizontal_resolution_pix == rhs.horizontal_resolution_pix &&
           lhs.vertical_resolution_pix == rhs.vertical_resolution_pix &&
           lhs.bit_rate_b_s == rhs.bit_rate_b_s && lhs.rotation_deg == rhs.rotation_deg &&
           lhs.uri == rhs.uri;
}

std::ostream &operator<<(std::ostream &str,
                         Camera::VideoStreamSettings const &video_stream_settings)
{
    return str << "[frame_rate_hz: " << video_stream_settings.frame_rate_hz
               << ", horizontal_resolution_pix: " << video_stream_settings.horizontal_resolution_pix
               << ", vertical_resolution_pix: " << video_stream_settings.vertical_resolution_pix
               << ", bit_rate_b_s: " << video_stream_settings.bit_rate_b_s
               << ", rotation_deg: " << video_stream_settings.rotation_deg
               << ", uri: " << video_stream_settings.uri << "]";
}

bool operator==(const Camera::VideoStreamInfo &lhs, const Camera::VideoStreamInfo &rhs)
{
    std::cerr << lhs.status << ", ..............., " << rhs.status << std::endl;
    return lhs.settings == rhs.settings && lhs.status == rhs.status;
}

std::ostream &operator<<(std::ostream &str, Camera::VideoStreamInfo const &video_stream_info)
{
    return str << "[" << std::endl
               << "settings: " << video_stream_info.settings << std::endl
               << "status: " << video_stream_info.status << std::endl
               << "]";
}

std::ostream &operator<<(std::ostream &str,
                         Camera::VideoStreamInfo::Status const &video_stream_info_status)
{
    switch (video_stream_info_status) {
        case Camera::VideoStreamInfo::Status::IN_PROGRESS:
            return str << "IN_PROGRESS";
        case Camera::VideoStreamInfo::Status::NOT_RUNNING:
            return str << "NOT_RUNNING";
        default:
            return str << "UNKNOWN";
    }
}

} // namespace dronecode_sdk
