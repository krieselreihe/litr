const { description } = require('../../package')

module.exports = {
  /**
   * Ref：https://v1.vuepress.vuejs.org/config/#title
   */
  title: 'Litr',
  /**
   * Ref：https://v1.vuepress.vuejs.org/config/#description
   */
  description,

  /**
   * Extra tags to be injected to the page HTML `<head>`
   *
   * ref：https://v1.vuepress.vuejs.org/config/#head
   */
  head: [
    ['meta', { name: 'theme-color', content: '#931bd8' }],
    ['meta', { name: 'apple-mobile-web-app-capable', content: 'yes' }],
    ['meta', { name: 'apple-mobile-web-app-status-bar-style', content: 'black' }],
  ],

  /**
   * Theme configuration, here is the default theme configuration for VuePress.
   *
   * ref：https://v1.vuepress.vuejs.org/theme/default-theme-config.html
   */
  themeConfig: {
    repo: 'krieselreihe/litr',
    editLinks: true,
    docsDir: 'src/website',
    logo: 'https://v1.vuepress.vuejs.org/hero.png',
    nav: [
      {
        text: 'Guide',
        link: '/guide/',
      },
      {
        text: 'Configuration',
        link: '/config/'
      },
      {
        text: 'FAQ',
        link: '/faq/'
      }
    ],
    sidebar: {
      '/guide/': [
        {
          title: 'Guide',
          collapsable: false,
          children: [
            '',
            'configure-commands',
            'child-commands',
            'parameters',
            'execution'
          ]
        }
      ],
    }
  }
}
