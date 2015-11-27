#include "common.h"
#include "out_stream.h"


out_stream::out_stream(enum AVCodecID Id){
  this->codecId = Id;
  this->is_open = 0;
  this->stream = NULL;
};  

out_stream::~out_stream(){
  //free the context and close the stuff
  if(this->is_open){
    this->close();
  }
}

int out_stream::open(AVFormatContext *av_format_context){

  AVCodecContext *codec_ctx = NULL;
  AVCodec        *codec = NULL;

  int result = 0;

  std::string errorMessage;
  
  /* find the codec for encoding */
  codec = avcodec_find_encoder(this->codecId);
  if (!codec) {
    errorMessage = "Video codec not found";
    this->setErrorMessage(errorMessage);
    return 1;
  }
  
  //create a new video stream
  /*We keep a direct pointer to the video stream, but you can still access
    it through the output_struct->av_format_context->streams structure */

  this->stream =  avformat_new_stream(av_format_context, codec);
  if (!this->stream) {
    errorMessage = "Could not alloc stream";
    this->setErrorMessage(errorMessage);
    return 1;
  }
  
  result = this->setup_codec();
  if (!result) {
    errorMessage = "Could not setup codec";
    this->setErrorMessage(errorMessage);
    return 1;
  }
  
  //some formats - like wmv - require a global header
  if (av_format_context->oformat->flags & AVFMT_GLOBALHEADER){
    codec_ctx->flags |= CODEC_FLAG_GLOBAL_HEADER;
  }

  /* open the codec */
  result = avcodec_open2(codec_ctx, codec, NULL);
  if (result < 0) {
    errorMessage =  "Video codec could not open codec\n";
    errorMessage.append("Error String: ");
    errorMessage.append(common::get_error_text(result));
    this->setErrorMessage(errorMessage);
    return 1;
  }

  return 0;
}

int out_stream::close(){
  
  /* close the codec */
  if(this->is_open && this->stream){
    avcodec_close(this->stream->codec);
    av_freep(this->stream->codec);
    av_freep(this->stream);
  }
  
  this->is_open = 0;
  return 0;
}

int out_stream::setup_codec(){
  return 0;
}

