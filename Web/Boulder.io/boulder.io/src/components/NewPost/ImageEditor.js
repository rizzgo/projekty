/**
 * @author xmikus18
 * strongly inspired:
 * https://www.npmjs.com/package/react-painter
 */

import React from 'react';
import { ReactPainter } from 'react-painter';
import './ImageEditor.css'

class Editor extends React.Component {
    state = {
      image: null
    };
  
    handleFileInputChange = ev => {
      const { target } = ev;
      const image = target.files ? target.files[0] : null;
  
      if (image) {
        this.setState({
          image
        });
      } else {
        console.error('image is null in handleImageSelected');
      }
    };
  
    render() {
      const { width, height, onSave } = this.props;
      const windowWidth = document.documentElement.clientWidth
      console.log(windowWidth)
  
      return this.state.image ? (
        <ReactPainter
          width={windowWidth}
          height={380}
          onSave={onSave}
          image={this.state.image}
          initialColor='#77fc03'
          
          render={({ triggerSave, getCanvasProps, imageDownloadUrl, setColor }) => (
            <div>

              {imageDownloadUrl ? (
                  <a href={imageDownloadUrl} download>
                  Download
                </a>
              ) : null}
              <div className='pallete'>
                <button className='set-green' onClick={() => {setColor("#77fc03")}}></button>
                <button className='set-blue' onClick={() => {setColor("#03d3fc")}}></button>
                <button className='set-orange' onClick={() => {setColor("#ffa500")}}></button>
              </div>
              <div className='canvas-container'>
                <canvas id="canvas-id" className='canvas' {...getCanvasProps()} />
              </div>
              <div className='save-image-container'>
                <button className='save-image' onClick={triggerSave}>Save Image</button>
              </div>
              {
                this.props.setEditedPicture(imageDownloadUrl),
                console.log("image:",imageDownloadUrl,"\n")
              }
              
            </div>
          )}
        />
      ) : (
          <div>
            <input className='file-input' id='image-input' type="file" onChange={this.handleFileInputChange} />
            <label className='file-input-label'for="image-input"> Select image </label>
          </div>
      );
    }
  }

export default Editor 