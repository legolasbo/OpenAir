import { createRadioButtons } from "./RadioButtons.js";

const getElementValueFrom = (info) => info.value ? `value="${info.value}"` : '';

const mapToRenderer = (type) => { 
    switch(type) {
        case "text": return (name, info) => `<input type="text" name="${name}" ${getElementValueFrom(info)} />`;
        case "hidden": return (name, info) => `<input type="hidden" name="${name}" ${getElementValueFrom(info)} />`;
        case "select": return (name, info) => createRadioButtons(name, info.options, info.value);
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
        .join("\n")}
    <input type="Submit" value="${submitLabel ? submitLabel : "Submit"}" />
    </form>
    `;
}

export {formFromOptions};