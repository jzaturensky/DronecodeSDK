#include <future>

#include "camera/camera.h"
#include "camera/camera.grpc.pb.h"

namespace dronecode_sdk {
namespace backend {

template<typename Camera = Camera>
class CameraServiceImpl final : public rpc::camera::CameraService::Service {
public:
    CameraServiceImpl(Camera &camera) : _camera(camera) {}

    grpc::Status TakePhoto(grpc::ServerContext * /* context */,
                           const rpc::camera::TakePhotoRequest * /* request */,
                           rpc::camera::TakePhotoResponse *response) override
    {
        auto camera_result = _camera.take_photo();

        if (response != nullptr) {
            fillResponseWithResult(response, camera_result);
        }

        return grpc::Status::OK;
    }

    template<typename ResponseType>
    void fillResponseWithResult(ResponseType *response,
                                dronecode_sdk::Camera::Result camera_result) const
    {
        auto rpc_result = static_cast<rpc::camera::CameraResult::Result>(camera_result);

        auto *rpc_camera_result = new rpc::camera::CameraResult();
        rpc_camera_result->set_result(rpc_result);
        rpc_camera_result->set_result_str(dronecode_sdk::Camera::result_str(camera_result));

        response->set_allocated_camera_result(rpc_camera_result);
    }

    grpc::Status StartPhotoInterval(grpc::ServerContext * /* context */,
                                    const rpc::camera::StartPhotoIntervalRequest *request,
                                    rpc::camera::StartPhotoIntervalResponse *response) override
    {
        if (request == nullptr) {
            if (response != nullptr) {
                fillResponseWithResult(response, dronecode_sdk::Camera::Result::WRONG_ARGUMENT);
            }

            return grpc::Status::OK;
        }

        auto camera_result = _camera.start_photo_interval(request->interval_s());

        if (response != nullptr) {
            fillResponseWithResult(response, camera_result);
        }

        return grpc::Status::OK;
    }

    grpc::Status StopPhotoInterval(grpc::ServerContext * /* context */,
                                   const rpc::camera::StopPhotoIntervalRequest * /* request */,
                                   rpc::camera::StopPhotoIntervalResponse *response) override
    {
        auto camera_result = _camera.stop_photo_interval();

        if (response != nullptr) {
            fillResponseWithResult(response, camera_result);
        }

        return grpc::Status::OK;
    }

    grpc::Status StartVideo(grpc::ServerContext * /* context */,
                            const rpc::camera::StartVideoRequest * /* request */,
                            rpc::camera::StartVideoResponse *response) override
    {
        auto camera_result = _camera.start_video();

        if (response != nullptr) {
            fillResponseWithResult(response, camera_result);
        }

        return grpc::Status::OK;
    }

    grpc::Status StopVideo(grpc::ServerContext * /* context */,
                           const rpc::camera::StopVideoRequest * /* request */,
                           rpc::camera::StopVideoResponse *response) override
    {
        auto camera_result = _camera.stop_video();

        if (response != nullptr) {
            fillResponseWithResult(response, camera_result);
        }

        return grpc::Status::OK;
    }

    grpc::Status StartVideoStreaming(grpc::ServerContext * /* context */,
                                     const rpc::camera::StartVideoStreamingRequest * /* request */,
                                     rpc::camera::StartVideoStreamingResponse *response) override
    {
        auto camera_result = _camera.start_video_streaming();

        if (response != nullptr) {
            fillResponseWithResult(response, camera_result);
        }

        return grpc::Status::OK;
    }

    grpc::Status StopVideoStreaming(grpc::ServerContext * /* context */,
                                    const rpc::camera::StopVideoStreamingRequest * /* request */,
                                    rpc::camera::StopVideoStreamingResponse *response) override
    {
        auto camera_result = _camera.stop_video_streaming();

        if (response != nullptr) {
            fillResponseWithResult(response, camera_result);
        }

        return grpc::Status::OK;
    }

    grpc::Status SetMode(grpc::ServerContext * /* context */,
                         const rpc::camera::SetModeRequest *request,
                         rpc::camera::SetModeResponse *response) override
    {
        if (request != nullptr) {
            auto camera_result = _camera.set_mode(translateRPCCameraMode(request->camera_mode()));

            if (response != nullptr) {
                fillResponseWithResult(response, camera_result);
            }
        }

        return grpc::Status::OK;
    }

    static dronecore::Camera::Mode translateRPCCameraMode(const rpc::camera::CameraMode mode)
    {
        switch (mode) {
            case rpc::camera::CameraMode::PHOTO:
                return dronecore::Camera::Mode::PHOTO;
            case rpc::camera::CameraMode::VIDEO:
                return dronecore::Camera::Mode::VIDEO;
            default:
                return dronecore::Camera::Mode::UNKNOWN;
        }
    }

    static rpc::camera::CameraMode translateCameraMode(const dronecore::Camera::Mode mode)
    {
        switch (mode) {
            case dronecore::Camera::Mode::PHOTO:
                return rpc::camera::CameraMode::PHOTO;
            case dronecore::Camera::Mode::VIDEO:
                return rpc::camera::CameraMode::VIDEO;
            default:
                return rpc::camera::CameraMode::UNKNOWN;
        }
    }

    grpc::Status SubscribeMode(grpc::ServerContext * /* context */,
                               const rpc::camera::SubscribeModeRequest * /* request */,
                               grpc::ServerWriter<rpc::camera::ModeResponse> *writer) override
    {
        std::promise<void> stream_closed_promise;
        auto stream_closed_future = stream_closed_promise.get_future();

        bool is_finished = false;

        _camera.subscribe_mode(
            [&writer, &stream_closed_promise, &is_finished](const dronecore::Camera::Mode mode) {
                rpc::camera::ModeResponse rpc_mode_response;
                rpc_mode_response.set_camera_mode(translateCameraMode(mode));

                if (!writer->Write(rpc_mode_response) && !is_finished) {
                    is_finished = true;
                    stream_closed_promise.set_value();
                }
            });

        stream_closed_future.wait();
        return grpc::Status::OK;
    }

    grpc::Status
    SetVideoStreamSettings(grpc::ServerContext * /* context */,
                           const rpc::camera::SetVideoStreamSettingsRequest *request,
                           rpc::camera::SetVideoStreamSettingsResponse * /* response */) override
    {
        if (request != nullptr) {
            const auto video_stream_settings =
                translateRPCVideoStreamSettings(request->video_stream_settings());
            _camera.set_video_stream_settings(video_stream_settings);
        }

        return grpc::Status::OK;
    }

    static std::unique_ptr<rpc::camera::VideoStreamSettings>
    translateVideoStreamSettings(const dronecore::Camera::VideoStreamSettings video_stream_settings)
    {
        auto rpc_video_stream_settings = std::unique_ptr<rpc::camera::VideoStreamSettings>(
            new rpc::camera::VideoStreamSettings());
        rpc_video_stream_settings->set_frame_rate_hz(video_stream_settings.frame_rate_hz);
        rpc_video_stream_settings->set_horizontal_resolution_pix(
            video_stream_settings.horizontal_resolution_pix);
        rpc_video_stream_settings->set_vertical_resolution_pix(
            video_stream_settings.vertical_resolution_pix);
        rpc_video_stream_settings->set_bit_rate_b_s(video_stream_settings.bit_rate_b_s);
        rpc_video_stream_settings->set_rotation_deg(video_stream_settings.rotation_deg);
        rpc_video_stream_settings->set_uri(video_stream_settings.uri);

        return rpc_video_stream_settings;
    }

    static dronecore::Camera::VideoStreamSettings translateRPCVideoStreamSettings(
        const rpc::camera::VideoStreamSettings &rpc_video_stream_settings)
    {
        dronecore::Camera::VideoStreamSettings video_stream_settings;
        video_stream_settings.frame_rate_hz = rpc_video_stream_settings.frame_rate_hz();
        video_stream_settings.horizontal_resolution_pix =
            rpc_video_stream_settings.horizontal_resolution_pix();
        video_stream_settings.vertical_resolution_pix =
            rpc_video_stream_settings.vertical_resolution_pix();
        video_stream_settings.bit_rate_b_s = rpc_video_stream_settings.bit_rate_b_s();
        video_stream_settings.rotation_deg = rpc_video_stream_settings.rotation_deg();
        video_stream_settings.uri = rpc_video_stream_settings.uri();

        return video_stream_settings;
    }

    static std::unique_ptr<rpc::camera::VideoStreamInfo>
    translateVideoStreamInfo(const dronecore::Camera::VideoStreamInfo &video_stream_info)
    {
        auto rpc_video_stream_info =
            std::unique_ptr<rpc::camera::VideoStreamInfo>(new rpc::camera::VideoStreamInfo());

        auto rpc_video_stream_settings = translateVideoStreamSettings(video_stream_info.settings);
        rpc_video_stream_info->set_allocated_video_stream_settings(
            rpc_video_stream_settings.release());

        auto rpc_video_stream_status = translateVideoStreamStatus(video_stream_info.status);
        rpc_video_stream_info->set_video_stream_status(rpc_video_stream_status);

        return rpc_video_stream_info;
    }

    static rpc::camera::VideoStreamInfo_VideoStreamStatus
    translateVideoStreamStatus(const dronecore::Camera::VideoStreamInfo::Status status)
    {
        switch (status) {
            case dronecore::Camera::VideoStreamInfo::Status::IN_PROGRESS:
                return rpc::camera::VideoStreamInfo_VideoStreamStatus_IN_PROGRESS;
            case dronecore::Camera::VideoStreamInfo::Status::NOT_RUNNING:
            default:
                return rpc::camera::VideoStreamInfo_VideoStreamStatus_NOT_RUNNING;
        }
    }

    static dronecore::Camera::VideoStreamInfo::Status
    translateRPCVideoStreamStatus(const rpc::camera::VideoStreamInfo_VideoStreamStatus status)
    {
        switch (status) {
            case rpc::camera::VideoStreamInfo_VideoStreamStatus_IN_PROGRESS:
                return dronecore::Camera::VideoStreamInfo::Status::IN_PROGRESS;
            case rpc::camera::VideoStreamInfo_VideoStreamStatus_NOT_RUNNING:
            default:
                return dronecore::Camera::VideoStreamInfo::Status::NOT_RUNNING;
        }
    }

    static dronecore::Camera::VideoStreamInfo
    translateRPCVideoStreamInfo(const rpc::camera::VideoStreamInfo &rpc_video_stream_info)
    {
        dronecore::Camera::VideoStreamInfo video_stream_info;
        video_stream_info.settings =
            translateRPCVideoStreamSettings(rpc_video_stream_info.video_stream_settings());
        video_stream_info.status =
            translateRPCVideoStreamStatus(rpc_video_stream_info.video_stream_status());

        return video_stream_info;
    }

    grpc::Status SubscribeVideoStreamInfo(
        grpc::ServerContext * /* context */,
        const rpc::camera::SubscribeVideoStreamInfoRequest * /* request */,
        grpc::ServerWriter<rpc::camera::VideoStreamInfoResponse> *writer) override
    {
        std::promise<void> stream_closed_promise;
        auto stream_closed_future = stream_closed_promise.get_future();

        bool is_finished = false;

        _camera.subscribe_video_stream_info(
            [&writer, &stream_closed_promise, &is_finished](
                const dronecore::Camera::VideoStreamInfo video_info) {
                rpc::camera::VideoStreamInfoResponse rpc_video_stream_info_response;
                auto video_stream_info = translateVideoStreamInfo(video_info);
                rpc_video_stream_info_response.set_allocated_video_stream_info(
                    video_stream_info.release());

                if (!writer->Write(rpc_video_stream_info_response) && !is_finished) {
                    is_finished = true;
                    stream_closed_promise.set_value();
                }
            });

        stream_closed_future.wait();

        return grpc::Status::OK;
    }

private:
    Camera &_camera;
};

} // namespace backend
} // namespace dronecode_sdk
