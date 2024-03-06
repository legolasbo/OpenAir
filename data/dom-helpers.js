
const createSubmitButton = (value) => createInput({type: "submit", value: value});

function createButton(label, callback) {
    const button = createElementWithAttributes("button", {type: "button"}, label);
    button.onclick = callback;
    return button;
}

const createDeleteButton = (action, uuid) => createElementWithAttributes(
    "form",
    {
        action: action,
        method: "post",
    }, 
    createHiddenInput("uuid", uuid),
    createSubmitButton("delete")
);

function createRadioButtons(name, options, idPrefix = "", selected = "") {
    const wrapper = document.createElement("div");
    
    for (const opt in options) {
        if (!Object.hasOwnProperty.call(options, opt)) {
            continue;
        }

        const buttonId = idPrefix ? `${idPrefix}_${name}_${opt}` : `${name}_${opt}`;

        const checked = Object.values(options).length == 1 || selected === opt ? {checked: true} : {};

        wrapper.appendChild(createInput({
            ...{
                type: "radio",
                name: name,
                value: opt,
                id: buttonId,
                required: true,
            },
            ...checked,
        }));
        wrapper.appendChild(createElementWithAttributes("label", {for: buttonId}, options[opt]));
    }

    return wrapper;
}

function createElementWithAttributes(type, attributes, ...children) {
    const element = document.createElement(type);
    for (const attribute in attributes) {
        if (!Object.hasOwnProperty.call(attributes, attribute)) {
            continue;
        }

        element.setAttribute(attribute, attributes[attribute])
    }

    for (const child of children) {
        element.append(child);
    }
    
    return element;
}

const createElement = (type, ...children) => createElementWithAttributes(type, {}, ...children);

const createInput = (attributes) => createElementWithAttributes("input", attributes);

const createTextInput = (name, value) => createInput({
    type: "text",
    name: name,
    value: value,
});
const createHiddenInput = (name, value) => createInput({
        type: "hidden",
        name: name,
        value: value,
    });
