import AbstractView from "../AbstractView.js";
import { formFromOptions } from "../form/Form.js";

export default class FileSystem extends AbstractView {
    constructor(params) {
        super(params);
    }

    async getHtml() {
        const response = await fetch("/api/filesystem/list");
        const items = await response.json();

        return `
            <h1>Filesystem</h1>
            <ul>
                ${items.map(value => `<li>${value} - <a href="/filesystem/delete?file=${value}">Delete</a></li>`)
                    .join("\n")
                }
            <ul>
        `;
    }
}
