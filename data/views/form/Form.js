import { createRadioButtons } from "./RadioButtons.js";

const getElementValueFrom = (info) => info.value ? `value="${info.value}"` : '';
const getNumberConstraintsFromInfo = (info) => {
    if (!info.constrain) {
        return "";
    }

    constraints = ["min", "max"];
    constraints.map(val => info.constrain[val] ? `${val}="${info.constrain[val]}"` : "");
    return constraints.join(" ");
}

const mapToRenderer = (type) => { 
    switch(type) {
        case "text": return (name, info) => `<input type="text" name="${name}" ${getElementValueFrom(info)} />`;
        case "hidden": return (name, info) => `<input type="hidden" name="${name}" ${getElementValueFrom(info)} />`;
        case "select": return (name, info) => createRadioButtons(name, info.options, info.value);
        case "number": return (name, info) => `<input type="number" name="${name}" ${getElementValueFrom(info)} ${getNumberConstraintsFromInfo(info)} />`
        default: return (name, info) => `<strong>Unknown form element type ${type} for ${name}</strong>`;
    }
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
    ${Object.entries(options)
        .map(([name, info]) => renderFormElement(name, info))
        .map(item => `<div class="form-item">${item}</div>`)
        .join("\n")}
    <input type="Submit" value="${submitLabel ? submitLabel : "Submit"}" />
    </form>
    `;
}

export {formFromOptions};