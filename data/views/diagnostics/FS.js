import AbstractView from "../AbstractView.js";

export default class FileSystem extends AbstractView {
    constructor(params) {
        super(params);
    }

    async getHtml() {
        const response = await fetch("/api/filesystem/list");
        const items = await response.json();

        return `
            <h1>Filesystem</h1>
            <p>Beware here be dragons. This section gives you the power to manipulate files on the filesystem and break your configuration and user interface as a result.</p>
            <p>Now that you are aware of the dangers, you can use this section to change/update your user interface without reflashing the filesystem image and thereby losing your configuration.</p>
            <p>You can also access your configuration file in order to back it up and/or restore it from a backup.</p>
            <ul>
                ${items
                    .sort()
                    .map(value => `<li><a href="/static${value}">${value}</a> - <a href="/filesystem/delete?file=${value}">Delete</a></li>`)
                    .join("\n")
                }
            <ul>
        `;
    }
}
