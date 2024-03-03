const contentElement = document.getElementById("content");

async function fetchJson(url) {
    const json = await fetch(url)
        .then(response => response.json());
    
    return json != null ? json : {};
}

const createSubmitButton = (value) => createInput({type: "submit", value: value});

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
        wrapper.appendChild(createElement("label", {for: buttonId}, options[opt]));
    }

    return wrapper;
}

function createElement(type, attributes, ...children) {
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

const createInput = (attributes) => createElement("input", attributes);

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


sensorsScreen();
