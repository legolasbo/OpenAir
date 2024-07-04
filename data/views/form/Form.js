import { createRadioButtons } from "./RadioButtons.js";

const getElementValueFrom = (info) => info.value ? `value="${info.value}"` : '';
const getNumberConstraintsFromInfo = (info) => {
    if (!info.constrain) {
        return "";
    }

    return ["min", "max"]
        .map(val => Object.keys(info.constrain).includes(val) ? `${val}="${info.constrain[val]}"` : "")
        .join(" ");
}

const mapToRenderer = (type) => { 
    switch(type) {
        case "text": return (name, info) => `<input type="text" name="${name}" ${getElementValueFrom(info)} />`;
        case "password": return (name, info) => `<input type="password" name="${name}" ${getElementValueFrom(info)} />`;
        case "hidden": return (name, info) => `<input type="hidden" name="${name}" ${getElementValueFrom(info)} />`;
        case "select": return (name, info) => createRadioButtons(name, info.options, info.value);
        case "number": return numberRenderer;
        case "boolean": return (name, info) => `<input type="checkbox" name="${name}" ${getElementValueFrom(info)} ${info.value == 1 ? "checked" : ""} />`;
        case "texts": return textsRenderer;
        default: return (name, info) => `<strong>Unknown form element type ${type} for ${name}</strong>`;
    }
}

const numberRenderer = (name, info) => {
    if (!info.constrain) {
        return `<input type="number" name="${name}" ${getElementValueFrom(info)} />`
    }
    
    return `<input type="range" name="${name}" id="${name}" ${getElementValueFrom(info)} ${getNumberConstraintsFromInfo(info)}><output id="${name}_output"></output>`
}

const textsRenderer = (name, info) => {
    return `
        <fieldset id="${name}Set">
            ${info.value
                .map(el => `<div><input type="text" name="${name}[]" value="${el}"/></div>`)
                .join("\n")
            }
            
            <button type="button" onclick="
const item = document.createElement('div');
const inp = document.createElement('input');
inp.setAttribute('type', 'text');
inp.setAttribute('name', '${name}[]');
item.appendChild(inp);
document
  .querySelector('#${name}Set button')
  .insertAdjacentElement('beforebegin', item)
            ">Add</button>
        </fieldset>
    `;
}

const renderFormElement = (name, info) => {
    const render = mapToRenderer(info.type);

    if (info.type !== "hidden" && info.label) {
        return `<strong>${info.label}</strong>${render(name,info)}`;
    }
    return render(name, info);
}


const formFromOptions = (action, options, submitLabel) => {
    return `
    <form action="${action}" method="post">
    ${options
        .map(({name, info}) => renderFormElement(name, info))
        .map(item => `<div class="form-item">${item}</div>`)
        .join("\n")}
    <input type="Submit" value="${submitLabel ? submitLabel : "Submit"}" />
    </form>
    `;
}

export {formFromOptions};