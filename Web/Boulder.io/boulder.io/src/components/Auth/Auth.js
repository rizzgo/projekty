/**
 * @author xseipe00
 */

export const setUsername = (username) => {
    console.log("setting:", username)
    localStorage.setItem("username", username)
}

export const getUsername = () => {
    //console.log("getting:", localStorage.getItem("username"))
    return localStorage.getItem("username")
}

export const isLoggedIn = () => {
    let username = getUsername()
    return (username !== null) && (typeof username !== "undefined")
}

export const logOut = () => {
    localStorage.removeItem("username")
}