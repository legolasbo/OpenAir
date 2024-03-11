
const createRadioButtons = (name, options, selected) => {
    return `
        ${
        Object.entries(options)
        .map(([id, label]) => {
            const checked = Object.values(options).length == 1 || selected === id ? "checked" : "";
            return `
            <input type="radio" name="${name}" value="${id}" required id="${name}_${id}" ${checked}/>
            <label for="${name}_${id}">${label}</label>
            `
        })
        }
    `;
};

export {
    createRadioButtons
};
