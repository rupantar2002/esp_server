// import CONFIG_JSON from "./config.json" assert { type: "json" };

console.log("this message is from app.js");

const myBtn = document.getElementById("my_button");
const myText = document.getElementById("my_text");

myBtn.addEventListener("click", () => {
  console.log("buton pushed");
  const xhr = new XMLHttpRequest();
  xhr.open("POST", "/");
  xhr.setRequestHeader("Content-Type", "application/json");
  xhr.send(myText.value);

  xhr.onload = () => {
    // const data = JSON.parse(xhr.response);
    console.log(xhr.response);
  };
});

// console.log(CONFIG_JSON);

// const LIST_TEMPLATE =
//   "\
//     < ul >\
//     <li>List item 1</li>\
//     </ul >\
// ";
// document.body.appendChild(create_input_box(LIST_TEMPLATE));
// document.body.appendChild(create_input_box(LIST_TEMPLATE));
// document.body.appendChild(create_input_box(LIST_TEMPLATE));

// function create_input_box(htmlCode) {
//   const template = document.createElement("name");
//   template.innerHTML = htmlCode.trim();
//   return template.firstElementChild;
// }
