/**
 * @author xmikus18
 */

import React, {useEffect,useState} from 'react';
import { useNavigate } from 'react-router-dom';
import Select from 'react-select'
import { getUsername } from '../Auth/Auth';
import { ReactPainter } from 'react-painter';
import './NewPostForm.css'
import ReactDom from "react-dom";
import { type } from 'os';
import Editor from './ImageEditor'


function NewPostForm(props) {
    
    const username = getUsername()
    const navigate = useNavigate()

    // constants for difficulty rating
    const numbers = [
        { value: '5', label: '5' },
        { value: '6', label: '6' },
        { value: '7', label: '7' },
        { value: '8', label: '8' },
    ]
    
    const letters = [
        {value: 'A', label: 'A' },
        {value: 'A+', label: 'A+' },
        {value: 'B', label: 'B' },
        {value: 'B+', label: 'B+' },
        {value: 'C', label: 'C' },
        {value: 'C+', label: 'C+' },
    ]

    let files
    let pictures = []
    
    // state declaration
    const [output , setOutput] = useState([])
    const [images, setImages] = useState([])
    const [description, setDescription] = useState()
    const [gradeNum , setGradeNum] = useState('')
    const [gradeAlp , setGradeAlp] = useState('')
    const [editedPicture, setEditedPicture] = useState()
    const [error, setError] = useState(false)
    const [editorOpen , setEditorOpen] = useState(false)

    function selectNumber(event)
    {
        setGradeNum(event.value)
    }

    function selectCharacter(event)
    {
        setGradeAlp(event.value)
    }

    function convertFileToBase64(file) {
        return new Promise((resolve, reject) => {
            let base64String = "";
            let reader = new FileReader();   
        
            reader.onload = function () {
                base64String = reader.result.replace("data:", "").replace(/^.+,/, "");
                resolve(base64String)
            }
            reader.readAsDataURL(file);
        })
    }

    async function getImages(event){
    
        files = event.target.files
        setImages(files)

        for(var index = 0; index < files.length;index++)
        {
            pictures[index] = await convertFileToBase64(files[index])
        }    
        setOutput([...output,...pictures])
    }

    function getDescription(props){
        setDescription(props.target.value)
    }

    // delete picture from array
    function deletePicture(event) {
        let array = output
        console.log(event.target.src)
        let index = array.indexOf(event.target.src)
        array.splice(index,1)
        setOutput([...array])
    }

    function savePicture() {
        let canvas = document.getElementById("canvas-id")
        let dataurl = canvas.toDataURL().replace("data:", "").replace(/^.+,/, "")
        console.log("dataurl", dataurl)
        setOutput([...output, dataurl])
        setEditorOpen(false)
    }

    // checks for post data and sends it to server
    async function sendNewPost(event) {
      event.preventDefault()
      console.log("data:",output)

      if(gradeNum === '' || gradeAlp === '' || output.length === 0) {
          console.log('error')
          setError(true)
          return;
      }

      const grade = gradeNum+gradeAlp

      let newPostObject = {
          username: username,
          grade: grade,
          votes: {},
          comments: [],
          rating: 0,
          description: description,
          images: output
      }

      const requestOptions = {
          method: 'POST',
          body: JSON.stringify(newPostObject),
          headers: { "Content-type":"application/json" }
        };
      const datas = []
      try{
          const res = await fetch("http://127.0.0.1:8000/posts",requestOptions)
          if (res.ok) {
              setError(false)
              navigate("/user")
            }
        }
      catch{
          console.log("error")
      }
    }
    const errorMessage = () => {
        return (
            <div
              className="errorRegister"
              style={{
                display: error ? '' : 'none',
              }}>
              Only description can be empty
              </div>
          );
    };

    const closeEditor = (e) => {
        if (e.target === e.currentTarget)
        {
            setEditorOpen(false)
        }
    }

    // show picture editor overlay
    const showEditor = () => {
        return editorOpen ? ( ReactDom.createPortal(

            <div className='overlay' onClick={closeEditor}>
                <div className='editorWindow'>
                    <Editor setEditedPicture={setEditedPicture}></Editor>
                </div>
            </div>
        ,document.getElementById('portal')
        )) : null 
    }

    // on change of edited picture save it
    useEffect(() => {
        if (editedPicture)
            savePicture()
    }, [editedPicture])
    
    return(
        <form className='adding-form'>
            <h2 className='form-header'>Choose:</h2>
            <div className='selectors'>
                <Select className='numbers' options={numbers} onChange={selectNumber} required/>
                <Select className='letters' options={letters} onChange={selectCharacter} required/>
            </div>

            <div className='image-group' id="img" >
                <div className='image-grid'>
                    {output.map(picture=>(<img id="output" className="image" src={"data:image/png;base64," + picture} onClick={deletePicture}/>))}
                </div>
            
                <div className='file-input-field'>
                    <input id="file-input" className='file-input' type="file" accept="image/*" onChange={getImages} multiple required/>
                    <label for="file-input"> Select images </label>
                </div>
            </div>
           <div>
                <button className='showEditorButton' onClick={(e) => {e.preventDefault(); setEditorOpen(!editorOpen)}}>Image Editor</button>

           </div>
            <div className="text-input-field">
                <textarea id="text-input" placeholder="Add description" className='text-input' type="text" onChange={getDescription}/>
           </div>

           <button onClick={sendNewPost} type="submit" className='add-post-button'>Add Post</button>
           <div>
               {errorMessage()}
            </div>
            <div>
                {showEditor()}
            </div>
        </form>
    )
}

export default NewPostForm;