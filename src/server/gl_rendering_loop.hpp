#ifndef __GL_RENDERING_LOOP_HPP__
#define __GL_RENDERING_LOOP_HPP__

#include "mesh_tile_renderer.hpp"
#include "shader_config.hpp"
#include "queue.hpp"

#include <boost/filesystem.hpp>
#include <future>


// This class serializes concurent tile rendering requests

class GLRenderingLoop {

public:
    GLRenderingLoop(): is_running_(false) {}

    // main rendering loop that processes a queue of jobs
    void run(const glsl::ProgramList &programs) {
        gl_.reset(new MeshTileRenderer(programs)) ;
        if ( !gl_->init() ) return ;

        is_running_ = true ;

        while (is_running_) {
            Job job ;
            try {
                job_queue_.pop(job) ;
                std::string bytes = gl_->render(job.x_, job.y_, job.z_, job.bytes_, job.options_) ;
                job.result_->set_value(bytes) ;
            }
            catch ( std::exception &) {
                return ;
            }
        }

        is_running_ = false ;
    }

    // add new job to the queue and obtain a promise to the result that you can wait upon

    std::shared_ptr<std::promise<std::string>> addJob(uint32_t x, uint32_t y, uint32_t z, const std::string &bytes,
                                                      const Dictionary &options) {

        std::shared_ptr<std::promise<std::string>> res(new std::promise<std::string>) ;
        job_queue_.push({x, y, z, bytes, options, res}) ;
        return res ;
    }

    void stop() {
        is_running_ = false ;
        job_queue_.stop() ;
    }

    struct Job {
        uint32_t x_, y_, z_ ;
        std::string bytes_ ;
        Dictionary options_ ;
        std::shared_ptr<std::promise<std::string>> result_ ;
    };

    Queue<Job> job_queue_ ;
    std::shared_ptr<MeshTileRenderer> gl_ ;
    std::atomic<bool> is_running_ ;
    boost::filesystem::path renderer_config_ ;
};



#endif
